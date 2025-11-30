#ifndef MYTMSUUI_LOGMSGHANDLER_H
#define MYTMSUUI_LOGMSGHANDLER_H

#include <QtLogging>
#include <QStandardPaths>
#include <QFile>
#include <stdio.h>

QtMessageHandler origLogMsgHandler = nullptr;

static const qint64 MAX_LOG_FILE_SIZE = 5000000; // 5 MB

FILE* openLogFileRotated(const QString& logFilePath, const QString& logFilePrevPath)
{
   QFile currLogFile(logFilePath);

   if (currLogFile.exists() && currLogFile.size() > MAX_LOG_FILE_SIZE)
   {
      // Time to rotate the log.  Move current log file to become the previous log file.

      // QFile::rename fails if the destination already exists.  Need to first remove
      // the existing file if that's the case.

      QFile prevLogFile(logFilePrevPath);
      if (prevLogFile.exists())
      {
         prevLogFile.remove();
      }

      currLogFile.rename(logFilePrevPath);
   }

   return fopen(qPrintable(logFilePath), "a");
}

void MyTMSUUI_LogMsgHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
   //// Format the log message
   QString formattedMsg = qFormatLogMessage(type, context, msg);

   //// Since these are static, they will only happen once.
   static QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                              + "/.my-tmsu-ui.log";
   static QString logFilePrevPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                                  + "/.my-tmsu-ui.prev.log";
   static FILE* logFilePtr = openLogFileRotated(logFilePath, logFilePrevPath);

   if (logFilePtr != NULL)
   {
      //// Write the formatted log message as a line to the log file
      fprintf(logFilePtr, "%s\n", qUtf8Printable(formattedMsg));
      fflush(logFilePtr);
   }
}

void MyTMSUUI_SetLoggingFormat()
{
   QString format;
   format += "%{time yyyy-MMM-dd hh:mm:ss.zzz}";
   //format += " // %{pid}.%{qthreadptr}";
   format += " (PID: %{pid})";
   format += " [";
   format += "%{if-debug}DEBUG%{endif}";
   format += "%{if-info}INFO %{endif}";
   format += "%{if-warning}WARN %{endif}";
   format += "%{if-critical}CRIT %{endif}";
   format += "%{if-fatal}FATAL%{endif}";
   format += "]";
   format += " %{file} %{line}";
   format += " { %{function} }";
   format += " %{message}";
   qSetMessagePattern(format);
}

#endif
