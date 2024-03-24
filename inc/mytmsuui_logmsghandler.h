#ifndef MYTMSUUI_LOGMSGHANDLER_H
#define MYTMSUUI_LOGMSGHANDLER_H

#include <QtLogging>
#include <QStandardPaths>
#include <stdio.h>

QtMessageHandler origLogMsgHandler = nullptr;

void MyTMSUUI_LogMsgHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
   //// Format the log message
   QString formattedMsg = qFormatLogMessage(type, context, msg);

   //// Since these are static, they will only happen once.
   static QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                              + "/.my-tmsu-ui.log"; // TODO: Investigate how to rotate the log files.
   static FILE* logFilePtr = fopen(qPrintable(logFilePath), "a");

   if (logFilePtr != NULL)
   {
      //// Write the formatted log message as a line to the log file
      fprintf(logFilePtr, "%s\n", qUtf8Printable(formattedMsg));
      fflush(logFilePtr);
   }
   // else
   // {
   //    printf("No logFilePtr\n");
   // }

   //// If a previous (default) handler existed, pass along to it for handling as well.
   // if (origLogMsgHandler)
   // {
   //    (*origLogMsgHandler)(type, context, msg);
   // }
}

#endif
