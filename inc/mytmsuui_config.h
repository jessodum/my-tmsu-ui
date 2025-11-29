#ifndef MYTMSUUI_CONFIG_H
#define MYTMSUUI_CONFIG_H

#include <QCommandLineParser>
#include <QSettings>

// Access configuration data.
//   Command Line Arg > env > conf file
//   (a.k.a.: QCommandLineParser > qgetenv > QSettings)

// Forward declaration(s)
class QCoreApplication;

class MyTMSUUI_Config
{

 public:

   static MyTMSUUI_Config* getInstance();

   // Copy ctor (unused)
   MyTMSUUI_Config(const MyTMSUUI_Config& other) = delete;

   // Copy assignment (unused)
   MyTMSUUI_Config& operator=(const MyTMSUUI_Config& other) = delete;

   void setApp(QCoreApplication* appPtr);

 protected:

 private:

   // (Default ctor)
   MyTMSUUI_Config();

   static MyTMSUUI_Config* ourInstancePtr;

   QCommandLineParser myCLArgParser;
   QSettings mySettings;
};

#endif
