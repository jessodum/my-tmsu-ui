#include "mytmsuui_mainwindow.h"
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
   MyTMSUUI_MainWindow theMainWin;
   MyTMSUUI_Data dataObj;

   theMainWin.setDataObj(&dataObj);

   //// Display the Main Window
   theMainWin.show();

   //// Run the GUI (Application)
   return theApp.exec();
}
