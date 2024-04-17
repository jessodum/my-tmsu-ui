#include "mytmsuui_interface.h"

#include "mytmsuui_data.h"
#include "mytmsuui_tagdata.h"
#include <QDirIterator>
#include <QTextStream>
#include <QRegularExpression>
#include <QtLogging>

//// --------------------------------------------------------------------------
MyTMSUUI_Interface::MyTMSUUI_Interface(QObject* parent)
 : QObject(parent)
 , myQueryTagsList()
 , myTagsToSetList()
 , myTagsToUnsetList()
 , myDataPtr(nullptr)
 , myIFProc()
 , myState(MyTMSUUI_IF_NS::Idle)
 , myErrorStr("")
 , myDBRootPath("")
 , myValuesIterIdx(0)
 , myCurrentValueIter("")
{
   myIFProc.setProgram("tmsu");

   //// -----------------
   //// Setup Connections
   //// -----------------
   connect(&myIFProc, SIGNAL(          finished(int, QProcess::ExitStatus)),
                this,   SLOT(handleFinishedProc(int, QProcess::ExitStatus)) );
}

//// --------------------------------------------------------------------------
MyTMSUUI_Interface::~MyTMSUUI_Interface()
{
   //// Nothing to do here (yet)
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::ensureNotRunning()
{
   if (myIFProc.state() == QProcess::NotRunning)
   {
      return;
   }

   //// else
   //// What's still running? (TODO-MAINT: Check myState?)

   //// End it
   myIFProc.terminate();
   myIFProc.waitForFinished(500);
   if (myIFProc.state() != QProcess::NotRunning)
   {
      //// Okay, that does it!  No more Mr. Nice Guy.
      myIFProc.kill();
   }

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::doNewBaseDir(const QString& newPath)
{
   ensureNotRunning();

   myIFProc.setWorkingDirectory(newPath);

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "info";

   myIFProc.setArguments(tmsuCmdArgs);

   myState = MyTMSUUI_IF_NS::InfoQuery;

   myIFProc.start(QIODeviceBase::ReadOnly);

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::retrieveFilesList(bool queryTagsSpecified)
{
   myState = MyTMSUUI_IF_NS::BuildFilesList;

   if (myDataPtr == nullptr)
   {
      myErrorStr = "No data object to store file listing";
      goIdle(true);
      return;
   }
   //// else

   bool useRecursion = myDataPtr->myRecurseEnabled;

   if (queryTagsSpecified)
   {
      ensureNotRunning();

      QStringList tmsuCmdArgs;
      tmsuCmdArgs << "files" << "--file" << "--path=.";

      //// TODO-FUTURE: Handle complex queries ( or, not, ()'s )
      ////              (see 'tmsu help files' for full list of operators)
      for (MyTMSUUI_TaggedValue queryTag : myQueryTagsList)
      {
         QString queryArg = queryTag.myTagName;
         if (!queryTag.myValue.isEmpty())
         {
            queryArg += "=";
            queryArg += queryTag.myValue;
         }
         tmsuCmdArgs << queryArg;
      }

      myIFProc.setArguments(tmsuCmdArgs);

      myIFProc.start(QIODeviceBase::ReadOnly);
      return;
   }
   else if (myDataPtr->myEmptyQueryAction == MyTMSUUI_IF_NS::RetrieveUntagged)
   {
      ensureNotRunning();

      QStringList tmsuCmdArgs;
      tmsuCmdArgs << "untagged" << "--no-dereference";

      if (!useRecursion)
      {
         tmsuCmdArgs << "--directory";
      }

      myIFProc.setArguments(tmsuCmdArgs);

      myIFProc.start(QIODeviceBase::ReadOnly);
      return;
   }
   //// else (Empty Query Action == RetrieveAll,
   ////       because MainWindow wouldn't call us for RetrieveNone)

   QStringList newFilesList;
   QDir tmpBaseDir(myDataPtr->myCurrentBaseDir);
   tmpBaseDir.setFilter(QDir::Files |
                        QDir::NoSymLinks |
                        QDir::NoDotAndDotDot |
                        QDir::Hidden |
                        QDir::CaseSensitive);
   tmpBaseDir.setSorting(QDir::Name |
                         QDir::DirsFirst);

   if (useRecursion)
   {
      QDirIterator dirIter(tmpBaseDir, QDirIterator::Subdirectories);
      while (dirIter.hasNext())
      {
         newFilesList << tmpBaseDir.relativeFilePath(dirIter.next());
      }
   }
   else
   {
      newFilesList = tmpBaseDir.entryList();
   }

   if (newFilesList.isEmpty())
   {
      myDataPtr->myCurrentFilesList.clear();
      myErrorStr = "No files found";
      goIdle(true);
      return;
   }

   myDataPtr->myCurrentFilesList = newFilesList;
   goIdle();
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::retrieveFileTags(const QString& filename)
{
   ensureNotRunning();

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "tags" << "-1" << "--color=always" << filename;

   myIFProc.setArguments(tmsuCmdArgs);

   myState = MyTMSUUI_IF_NS::RetrieveFileTags;

   myIFProc.start(QIODeviceBase::ReadOnly);

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::setTags(const QString& filename)
{
   myState = MyTMSUUI_IF_NS::SetTags;

   //// Make sure there are tags to set
   if (myTagsToSetList.size() == 0)
   {
      //// No tags to set. Any tags to unset?
      if (myTagsToUnsetList.size() > 0)
      {
         unsetTags(filename);
      }
      else
      {
         myErrorStr = "No tags to set or unset";
         goIdle(true);
      }
      return;
   }
   //// else

   ensureNotRunning();

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "tag" << filename;

   for (QString tag : myTagsToSetList)
   {
      tmsuCmdArgs << tag;
   }

   myIFProc.setArguments(tmsuCmdArgs);

   myIFProc.start(QIODeviceBase::ReadOnly);

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::unsetTags(const QString& filename)
{
   myState = MyTMSUUI_IF_NS::UnsetTags;

   //// Make sure there are tags to set
   if (myTagsToUnsetList.size() == 0)
   {
      //// No tags to unset. This is probably okay.
      myErrorStr = "";
      goIdle();
      return;
   }
   //// else

   ensureNotRunning();

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "untag" << filename;

   for (QString tag : myTagsToUnsetList)
   {
      tmsuCmdArgs << tag;
   }

   myIFProc.setArguments(tmsuCmdArgs);

   myIFProc.start(QIODeviceBase::ReadOnly);

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::doTagsDBQuery()
{
   ensureNotRunning();

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "tags" << "-1";

   myIFProc.setArguments(tmsuCmdArgs);

   myState = MyTMSUUI_IF_NS::TagsDBQuery;

   myIFProc.start(QIODeviceBase::ReadOnly);

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::doAllValuesDBQuery()
{
   ensureNotRunning();

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "values" << "-1";

   myIFProc.setArguments(tmsuCmdArgs);

   myState = MyTMSUUI_IF_NS::AllValuesDBQuery;

   myIFProc.start(QIODeviceBase::ReadOnly);

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::doNextTagsByValueDBQuery()
{
   if ((qsizetype)myValuesIterIdx >= (myDataPtr->myValuesList.size()) )
   {
      //// Reached the end of the Values list.
      //// Move on to next step in the sequence.
      doImpliesDBQuery();
      return;
   }
   //// else

   myCurrentValueIter = myDataPtr->myValuesList.at(myValuesIterIdx);

   ensureNotRunning();

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "tags" << "-1" << "--value" << myCurrentValueIter;

   myIFProc.setArguments(tmsuCmdArgs);

   myState = MyTMSUUI_IF_NS::TagsByValueDBQuery;

   myIFProc.start(QIODeviceBase::ReadOnly);

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::doImpliesDBQuery()
{
   ensureNotRunning();

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "imply";

   myIFProc.setArguments(tmsuCmdArgs);

   myState = MyTMSUUI_IF_NS::ImpliesDBQuery;

   myIFProc.start(QIODeviceBase::ReadOnly);

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedProc(int exitCode, QProcess::ExitStatus howExited)
{
   if (howExited == QProcess::CrashExit)
   {
      //// TODO-MAINT: Handle crashed "tmsu"?
      return;
   }

   switch(myState)
   {
      case MyTMSUUI_IF_NS::InfoQuery:
         handleFinishedInfoQuery(exitCode);
         break;

      case MyTMSUUI_IF_NS::TagsDBQuery:
         handleFinishedTagsDBQuery(exitCode);
         break;

      case MyTMSUUI_IF_NS::AllValuesDBQuery:
         handleFinishedAllValuesDBQuery(exitCode);
         break;

      case MyTMSUUI_IF_NS::TagsByValueDBQuery:
         handleFinishedTagsByValueDBQuery(exitCode);
         break;

      case MyTMSUUI_IF_NS::ImpliesDBQuery:
         handleFinishedImpliesDBQuery(exitCode);
         break;

      case MyTMSUUI_IF_NS::RetrieveFileTags:
         handleFinishedRetrieveFileTags(exitCode);
         break;

      case MyTMSUUI_IF_NS::BuildFilesList:
         handleFinishedBuildFilesList(exitCode);
         break;

      case MyTMSUUI_IF_NS::SetTags:
         handleFinishedSetTags(exitCode);
         break;

      case MyTMSUUI_IF_NS::UnsetTags:
         handleFinishedUnsetTags(exitCode);
         break;

      default:
         qWarning("Unhandled state %d (exit code %d)", myState, exitCode);
         break;
   }

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedInfoQuery(int exitCode)
{
   if (exitCode != 0)
   {
      myDBRootPath = "";
      myErrorStr = "No (tags) database in directory tree";
      goIdle(true);
      return;
   }
   //// else

   //// Command Success
   myErrorStr = "";

   //// Has the database root path changed?
   QString outputRootPath = myDBRootPath;

   QTextStream procOutputStream(myIFProc.readAllStandardOutput());
   QString outputLine;
   while (procOutputStream.readLineInto(&outputLine))
   {
      //// Skip lines that don't start with "Root path: "
      if (!outputLine.startsWith("Root path: "))
      {
         continue;
      }
      //// else
      //// "Root path: "
      ////  12345678901 = 11 characters
      outputRootPath = outputLine.last(outputLine.size() - 11);

      //// Skip remaining output
      break;
   }

   if (outputRootPath != myDBRootPath)
   {
      //// New database
      myDBRootPath = outputRootPath;

      //// Begin the sequence to (re-)build our tags list
      doTagsDBQuery();
   }
   else
   {
      //// Move on to updating list of image files based on new directory
      //// (Note: the main window will handle calling retrieveFilesList from
      ////        its "interface idle" handling slot, because it will know
      ////        whether any query tags are selected.)
      goIdle();
   }

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedTagsDBQuery(int exitCode)
{
   if (exitCode != 0)
   {
      myErrorStr = "Error getting list of Tags from database";
      goIdle(true);
      return;
   }
   //// else

   //// Command Success
   myErrorStr = "";

   //// First, clear out (any) old tags from our data list
   myDataPtr->clearTagsList();

   QTextStream procOutputStream(myIFProc.readAllStandardOutput());
   QString outputLine;
   while (procOutputStream.readLineInto(&outputLine))
   {
      myDataPtr->myTagsList.append(new MyTMSUUI_TagData(outputLine));
   }

   //// Next step in the sequence
   doAllValuesDBQuery();

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedAllValuesDBQuery(int exitCode)
{
   if (exitCode != 0)
   {
      myErrorStr = "Error getting list of Values from database";
      goIdle(true);
      return;
   }
   //// else

   //// Command Success
   myErrorStr = "";

   //// First, clear out (any) old values from our data list
   myDataPtr->myValuesList.clear();

   QTextStream procOutputStream(myIFProc.readAllStandardOutput());
   QString outputLine;
   while (procOutputStream.readLineInto(&outputLine))
   {
      myDataPtr->myValuesList.append(outputLine);
   }

   //// Initialize iteration
   myValuesIterIdx = 0;

   //// Next step in the sequence
   doNextTagsByValueDBQuery();

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedTagsByValueDBQuery(int exitCode)
{
   if (exitCode != 0)
   {
      myErrorStr = "Error getting Tags using a Value";
      goIdle(true);
      return;
   }
   //// else

   //// Command Success
   myErrorStr = "";

   QTextStream procOutputStream(myIFProc.readAllStandardOutput());
   QString outputLine;
   while (procOutputStream.readLineInto(&outputLine))
   {
      if (outputLine == (myCurrentValueIter + ":"))
      {
         //// Skip output that's echoing the Value
         continue;
      }

      //// Find the Tag data in our list and add the Value to its list
      MyTMSUUI_TagData* tagDataPtr = MyTMSUUI_TagData::findInListOfPointers(myDataPtr->myTagsList, outputLine);
      if (tagDataPtr != nullptr)
      {
         tagDataPtr->addTagValue(myCurrentValueIter);
      }
      else
      {
         qWarning("Could not find Tag Data in list for Tag %s", qUtf8Printable(outputLine));
      }
   }

   //// Move on to next Value in our list
   myValuesIterIdx++;
   doNextTagsByValueDBQuery();

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedImpliesDBQuery(int exitCode)
{
   if (exitCode != 0)
   {
      myErrorStr = "Error getting list of implied Tags from database";
      goIdle(true);
      return;
   }
   //// else

   //// Command Success
   myErrorStr = "";

   QRegularExpression regex("^\\s*([-\\w]+)(=([-\\w]+))? -> ([-\\w]+)(=([-\\w]+))?\\s*$");

   QTextStream procOutputStream(myIFProc.readAllStandardOutput());
   QString outputLine;
   while (procOutputStream.readLineInto(&outputLine))
   {
      QRegularExpressionMatch match = regex.match(outputLine);
      if(match.hasMatch())
      {
         QString impliesTag = match.captured(1);
         QString impliedTag = match.captured(4);

         //// Find the Tag data for the "implies" Tag in our list
         MyTMSUUI_TagData* impliesTagDataPtr = MyTMSUUI_TagData::findInListOfPointers(myDataPtr->myTagsList, impliesTag);

         if (!(match.captured(2).isNull()))
         {
            impliesTagDataPtr->addTagValue(match.captured(3));
         }

         //// Find the Tag data for the "implied" Tag in our list
         MyTMSUUI_TagData* impliedTagDataPtr = MyTMSUUI_TagData::findInListOfPointers(myDataPtr->myTagsList, impliedTag);

         if (!(match.captured(5).isNull()))
         {
            impliedTagDataPtr->addTagValue(match.captured(6));
         }

         //// Add imply connection
         impliesTagDataPtr->implies(impliedTagDataPtr);
QString dbImpliesTag(impliesTagDataPtr->getTagName());
if (impliesTagDataPtr->getValuesList().size() > 0)
{
	dbImpliesTag += "=";
	dbImpliesTag += impliesTagDataPtr->getValuesList()[0];
}
QString dbImpliedTag(impliedTagDataPtr->getTagName());
if (impliedTagDataPtr->getValuesList().size() > 0)
{
	dbImpliedTag += "=";
	dbImpliedTag += impliedTagDataPtr->getValuesList()[0];
}
qDebug("JDO: %s -> %s", qUtf8Printable(dbImpliesTag), qUtf8Printable(dbImpliedTag));
      }
   }

   //// Done with the sequence
   goIdle();

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedRetrieveFileTags(int exitCode)
{
   if (exitCode != 0)
   {
      myErrorStr = "Error retrieving tags for the filename";
      //// TODO-MAINT: Get error message from stderr?
      goIdle(true);
      return;
   }
   //// else

   //// Command Success
   myErrorStr = "";
   myDataPtr->myCurrentImgTaggedValList.clear();

   QRegularExpression regex("^([-\\w]+)(=([-\\w]+))?$");

   QByteArray outputBytes(myIFProc.readAllStandardOutput());

   QString tagLine;
   QTextStream tagLineStream(&tagLine);
   bool firstLine = true; //// First line of output is the filename

   QString escapeCode;
   QTextStream escapeCodeStream(&escapeCode);
   bool readingEscapeCode = false;

   MyTMSUUI_Tagged_NS::CheckedState currentCheckedState
      = MyTMSUUI_Tagged_NS::SetExplicitTag;

   for (char byte : outputBytes)
   {
      if (byte == 0x0a) //// '\n'
      {
         if (firstLine)
         {
            //// Just ignore first line
            firstLine = false;
         }
         else
         {
            MyTMSUUI_TaggedValue newTaggedValue;
            newTaggedValue.myCheckedState = currentCheckedState;

            //// Parse tagLine
            QRegularExpressionMatch match = regex.match(tagLine);
            if(match.hasMatch())
            {
               newTaggedValue.myTagName = match.captured(1);
               if ( match.hasCaptured(2) || (match.captured(2).length() > 0) )
               {
                  newTaggedValue.myValue = match.captured(3);
               }

               myDataPtr->myCurrentImgTaggedValList << newTaggedValue;
            }
            else
            {
               qWarning("tags output unrecognized: %s", qUtf8Printable(tagLine));
            }
         }

         //// Reset
         tagLine = "";
         currentCheckedState = MyTMSUUI_Tagged_NS::SetExplicitTag;
      }
      else if (readingEscapeCode)
      {
         if (byte == 'm')
         {
            readingEscapeCode = false;

            //// Parse escapeCode
            if (escapeCode == "33")
            {
               currentCheckedState = MyTMSUUI_Tagged_NS::SetBothTag;
            }
            else if (escapeCode == "36")
            {
               currentCheckedState = MyTMSUUI_Tagged_NS::SetImpliedTag;
            }
            //// else (escapeCode == "0") >>> "reset"
         }
         else if (byte != '[')
         {
            //// Should be a numeric digit
            escapeCodeStream << byte;
         }
      }
      else if (byte == 0x1b) //// <esc>
      {
         readingEscapeCode = true;
         escapeCode = ""; //// Reset
      }
      else
      {
         //// Not part of the escape code, so must be part of tagLine.
         tagLineStream << byte;
      }

   } //// End foreach output byte

   goIdle();

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedBuildFilesList(int exitCode)
{
   if (exitCode != 0)
   {
      myErrorStr = "Error retrieving files from query";
      //// TODO-MAINT: Get error message from stderr?
      goIdle(true);
      return;
   }
   //// else

   //// Command Success
   myErrorStr = "";

   QStringList newFilesList;
   bool useRecursion = myDataPtr->myRecurseEnabled;

   QTextStream procOutputStream(myIFProc.readAllStandardOutput());
   QString outputLine;
   while (procOutputStream.readLineInto(&outputLine))
   {
      //// Remove the "./" from the start of the string
      QString filteredString = outputLine.right(outputLine.size() - 2);

      //// Filter out recursed files when "useRecursion" is false
      if (useRecursion || !filteredString.contains('/'))
      {
         //// Include the file in the list
         newFilesList << filteredString;
      }
   }

   if (newFilesList.isEmpty())
   {
      myDataPtr->myCurrentFilesList.clear();
      myErrorStr = "No files matching query";
      goIdle(true);
      return;
   }

   myDataPtr->myCurrentFilesList = newFilesList;
   goIdle();
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedSetTags(int exitCode)
{
   if (exitCode != 0)
   {
      myErrorStr = "Error setting tags";
      //// TODO-MAINT: Get error message from stderr?
      ////   (NOTE: There may have been a partial command success in setting tags)
      goIdle(true);
      return;
   }
   //// else

   //// Command Success
   myErrorStr = "";

   //// Shouldn't be anything to read from stdout.
   //// Move on to unsetting tags...
   if (myTagsToUnsetList.size() == 0)
   {
      //// No tags to unset... all done!
      goIdle();
      return;
   }
   //// else

   if (myDataPtr == nullptr)
   {
      myErrorStr = "Cannot get current image filename (no data object)";
      goIdle(true);
      return;
   }
   //// else

   unsetTags(myDataPtr->getCurrentFilename());
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedUnsetTags(int exitCode)
{
   if (exitCode != 0)
   {
      //// This is likely to happen when ToBeSetImpliesTag is from a
      //// ToBeSetExplicitTag, rather than from a ToBeUnchecked causing
      //// SetBothTag --> ToBeSetImpliesTag.

      //// In other words, it might not be a real error.

      qWarning("Error unsetting tags");
      //// TODO-MAINT: Get error message from stderr?
      ////   (NOTE: There may have been a partial command success in unsetting tags)
   }

   myErrorStr = "";

   //// Shouldn't need to read anything from stdout.
   goIdle();
   return;
}

