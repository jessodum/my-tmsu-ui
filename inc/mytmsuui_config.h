#ifndef MYTMSUUI_CONFIG_H
#define MYTMSUUI_CONFIG_H

#include <QCommandLineParser>
#include <QSettings>

// Access configuration data.
//   Command Line Arg > env > conf file
//   (a.k.a.: QCommandLineParser > qgetenv > QSettings)

// Forward declaration(s)
class QCoreApplication;

namespace MyTMSUUI_ConfigNames
{
   // Each config name should come with a description (_DESCR) and value name (_VALNAME)
   // for the command line parser.
   static const QString MAX_IMAGE_HEIGHT = "maxImageHeight";
   static const QString MAX_IMAGE_HEIGHT_DESCR = "Images taller than <height> pixels are scaled to fit";
   static const QString MAX_IMAGE_HEIGHT_VALNAME = "height";
}

namespace MyTMSUUI_ConfigDefaults
{
   static const int DFLT_MAX_IMAGE_HEIGHT = 540;
}

class MyTMSUUI_Config
{

 public:

   static MyTMSUUI_Config* getInstance();

   // Copy ctor (unused)
   MyTMSUUI_Config(const MyTMSUUI_Config& other) = delete;

   // Copy assignment (unused)
   MyTMSUUI_Config& operator=(const MyTMSUUI_Config& other) = delete;

   // Uses the app object to process the command line arguments
   void parseArgs(QCoreApplication& app);

   // Similar to QSettings::value(...), returns value for specified
   // config name.  If no default value specified, returned value
   // is equivalent to empty string ("").
   QVariant getValue(const QString& configName);
   QVariant getValue(const QString& configName, const QVariant& dfltValue);

 protected:

 private:

   // (Default ctor)
   MyTMSUUI_Config();

   static MyTMSUUI_Config* ourInstancePtr;

   QCommandLineParser myCLArgParser;
   QSettings mySettings;
};

#endif
