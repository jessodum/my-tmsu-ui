#include "mytmsuui_interface.h"
#include "mytmsuui_data.h"
#include <QTextStream>
#include <stdio.h> // TODO: remove debug printf's

// ----------------------------------------------------------------------------
MyTMSUUI_Interface::MyTMSUUI_Interface(QObject* parent)
 : QObject(parent)
 , myDataPtr(nullptr)
 , myIFProc()
 , myState(MyTMSUUI_IF_NS::Idle)
 , myErrorStr("")
 , myDBRootPath("")
{
   myIFProc.setProgram("tmsu");

   // -----------------
   // Setup Connections
   // -----------------
   connect(&myIFProc, SIGNAL(          finished(int, QProcess::ExitStatus)),
                this,   SLOT(handleFinishedProc(int, QProcess::ExitStatus)) );
}

// ----------------------------------------------------------------------------
MyTMSUUI_Interface::~MyTMSUUI_Interface()
{
   // Nothing to do here (yet)
}

// ----------------------------------------------------------------------------
void MyTMSUUI_Interface::doNewBaseDir(const QString& newPath)
{
   if (myIFProc.state() != QProcess::NotRunning)
   {
      // What's still running? (TODO: Check myState?)

      // End it
      myIFProc.terminate();
      myIFProc.waitForFinished(500);
      if (myIFProc.state() != QProcess::NotRunning)
      {
         // Okay, that does it!  No more Mr. Nice Guy.
         myIFProc.kill();
      }
   }

   myIFProc.setWorkingDirectory(newPath);

   QStringList tmsuCmdArgs;
   tmsuCmdArgs << "info";

   myIFProc.setArguments(tmsuCmdArgs);

   myState = MyTMSUUI_IF_NS::InfoQuery;

   myIFProc.start(QIODeviceBase::ReadOnly);

   return;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedProc(int exitCode, QProcess::ExitStatus howExited)
{
   if (howExited == QProcess::CrashExit)
   {
      // TODO: Handle crashed "tmsu"?
      return;
   }

   switch(myState)
   {
      case MyTMSUUI_IF_NS::InfoQuery:
         handleFinishedInfoQuery(exitCode);
         break;

      default:
         printf("TODO: handleFinishedProc - Unhandled state %d (exit code %d)", myState, exitCode);
         break;
   }

   return;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_Interface::handleFinishedInfoQuery(int exitCode)
{
   if (exitCode == 0)
   {
      // InfoQuery Success

      // Has the database root path changed?
      QString outputRootPath = myDBRootPath;

      QTextStream procOutputStream(myIFProc.readAllStandardOutput());
      QString outputLine;
      while (procOutputStream.readLineInto(&outputLine))
      {
         // Skip lines that don't start with "Root path: "
         if (!outputLine.startsWith("Root path: "))
         {
            continue;
         }
         // else
         // "Root path: "
         //  12345678901 = 11 characters
         outputRootPath = outputLine.last(outputLine.size() - 11);

         // Skip remaining output
         break;
      }

      if (outputRootPath != myDBRootPath)
      {
         // New database
         // TODO: Move on to getting all tags & values from database
      }
      // else?
      // TODO: Move on to updating list of image files based on new directory
myErrorStr = "";
goIdle();
   }
   else
   {
      myDBRootPath = "";
      myErrorStr = "No (tags) database in directory tree";
      goIdle(true);
   }

   return;
}
