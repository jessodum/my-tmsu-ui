#include "mytmsuui_mainwindow.h"
#include "mytmsuui_consts.h"
#include "mytmsuui_config.h"
#include "mytmsuui_data.h"
#include "mytmsuui_logmsghandler.h"

#include <QApplication>

int main(int argc, char* argv[])
{
   //// Use the custom log message handler from "mytmsuui_logmsghandler.h"
   origLogMsgHandler = qInstallMessageHandler(MyTMSUUI_LogMsgHandler);

   MyTMSUUI_SetLoggingFormat(); //// Custom log line formatting

   qInfo("App Starting...");
   QApplication theApp(argc, argv);

   QCoreApplication::setOrganizationName(MyTMSUUI_Consts::APP_NAME_CLI);
   QCoreApplication::setApplicationName(MyTMSUUI_Consts::APP_NAME_CLI);
   QCoreApplication::setApplicationVersion(MyTMSUUI_Consts::APP_VERSION);

   // Initialize the Config singleton and parse the command line arguments
   // (need the app for the command line parser)
   MyTMSUUI_Config::getInstance()->parseArgs(theApp);

   MyTMSUUI_MainWindow theMainWin;
   MyTMSUUI_Data dataObj;

   theMainWin.setDataObj(&dataObj);

   //// Display the Main Window
   theMainWin.show();

   //// Run the GUI (Application)
   return theApp.exec();
}
