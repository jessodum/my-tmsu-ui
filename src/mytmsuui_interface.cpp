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
   //// What's still running? (TODO-FUTURE: Check myState?)

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

   QStringList newFilesList;

   if (queryTagsSpecified)
   {
      //// TODO: use "tmsu files" query
   }
   else
   {
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
      //// TODO-FUTURE: Handle crashed "tmsu"?
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

      default:
         qWarning("Unhandled state %d (exit code %d)", myState, exitCode);
         break;
   }

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedInfoQuery(int exitCode)
{
   if (exitCode == 0)
   {
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
   }
   else
   {
      myDBRootPath = "";
      myErrorStr = "No (tags) database in directory tree";
      goIdle(true);
   }

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedTagsDBQuery(int exitCode)
{
   if (exitCode == 0)
   {
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
   }
   else
   {
      myErrorStr = "Error getting list of Tags from database";
      goIdle(true);
   }

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedAllValuesDBQuery(int exitCode)
{
   if (exitCode == 0)
   {
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
   }
   else
   {
      myErrorStr = "Error getting list of Values from database";
      goIdle(true);
   }

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedTagsByValueDBQuery(int exitCode)
{
   if (exitCode == 0)
   {
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
   }
   else
   {
      myErrorStr = "Error getting Tags using a Value";
      goIdle(true);
   }

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedImpliesDBQuery(int exitCode)
{
   if (exitCode == 0)
   {
      //// Command Success
      myErrorStr = "";

      QRegularExpression regex("^\\s*([-\\w]+) -> ([-\\w]+)\\s*$");

      QTextStream procOutputStream(myIFProc.readAllStandardOutput());
      QString outputLine;
      while (procOutputStream.readLineInto(&outputLine))
      {
         QRegularExpressionMatch match = regex.match(outputLine);
         if(match.hasMatch())
         {
            QString impliesTag = match.captured(1);
            QString impliedTag = match.captured(2);

            //// Find the Tag data for the "implies" Tag in our list
            MyTMSUUI_TagData* impliesTagDataPtr = MyTMSUUI_TagData::findInListOfPointers(myDataPtr->myTagsList, impliesTag);

            //// Find the Tag data for the "implied" Tag in our list
            MyTMSUUI_TagData* impliedTagDataPtr = MyTMSUUI_TagData::findInListOfPointers(myDataPtr->myTagsList, impliedTag);

            //// Add imply connection
            impliesTagDataPtr->implies(impliedTagDataPtr);
         }
      }

      //// Done with the sequence
      goIdle();
   }
   else
   {
      myErrorStr = "Error getting list of implied Tags from database";
      goIdle(true);
   }

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedRetrieveFileTags(int exitCode)
{
   if (exitCode == 0)
   {
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
      }

      goIdle();
   }
   else
   {
      myErrorStr = "Error retrieving tags for the filename";
      //// TODO-MAINT: Get error message from stderr?
      goIdle(true);
   }

   return;
}

