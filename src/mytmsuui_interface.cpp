#include "mytmsuui_interface.h"
#include "mytmsuui_data.h"
#include <QTextStream>
#include <QtLogging>

//// --------------------------------------------------------------------------
MyTMSUUI_Interface::MyTMSUUI_Interface(QObject* parent)
 : QObject(parent)
 , myDataPtr(nullptr)
 , myIFProc()
 , myState(MyTMSUUI_IF_NS::Idle)
 , myErrorStr("")
 , myDBRootPath("")
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
   //// What's still running? (TODO: Check myState?)

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
   ensureNotRunning();

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "tags" << "-1" << "--value" << "TODO"; //// TODO

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
      //// TODO: Handle crashed "tmsu"?
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

      default:
         qDebug("TODO: handleFinishedProc - Unhandled state %d (exit code %d)", myState, exitCode);
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
         doTagsDBQuery();
      }
      else
      {
      //// TODO: Move on to updating list of image files based on new directory
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

      //// TODO:
goIdle();
   }
   else
   {
      myErrorStr = "TODO Error";
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

      //// TODO:
goIdle();
   }
   else
   {
      myErrorStr = "TODO Error";
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

      //// TODO:
goIdle();
   }
   else
   {
      myErrorStr = "TODO Error";
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

      //// TODO:
goIdle();
   }
   else
   {
      myErrorStr = "TODO Error";
      goIdle(true);
   }

   return;
}

