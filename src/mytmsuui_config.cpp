#include "mytmsuui_config.h"
#include "mytmsuui_consts.h"
#include <QCoreApplication>

static const QString ENV_VAR_NAME_PREFIX = "MTU_";

MyTMSUUI_Config* MyTMSUUI_Config::ourInstancePtr = nullptr;

//// --------------------------------------------------------------------------
MyTMSUUI_Config* MyTMSUUI_Config::getInstance()
{
   if (ourInstancePtr == nullptr)
   {
      ourInstancePtr = new MyTMSUUI_Config();
   }

   return ourInstancePtr;
}

//// --------------------------------------------------------------------------
MyTMSUUI_Config::MyTMSUUI_Config()
{
   myCLArgParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
   myCLArgParser.setApplicationDescription(MyTMSUUI_Consts::APP_DESCRIPTION);
   myCLArgParser.addHelpOption();
   myCLArgParser.addVersionOption();

   // Set up the configurable options for command line parser
   myCLArgParser.addOptions({
      {
         MyTMSUUI_ConfigNames::MAX_IMAGE_HEIGHT,
         MyTMSUUI_ConfigNames::MAX_IMAGE_HEIGHT_DESCR,
         MyTMSUUI_ConfigNames::MAX_IMAGE_HEIGHT_VALNAME
      }
      // ,{
      //    MyTMSUUI_ConfigNames::ANOTHER_CONFIG,
      //    MyTMSUUI_ConfigNames::ANOTHER_CONFIG_DESCR,
      //    MyTMSUUI_ConfigNames::ANOTHER_CONFIG_VALNAME
      // }
   });
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Config::parseArgs(QCoreApplication& app)
{
   static bool doneOnce = false;

   if (doneOnce)
   {
      qWarning("Command line args have already been parsed for config");
      return;
   }

   myCLArgParser.process(app);
   doneOnce = true;
}

//// --------------------------------------------------------------------------
QVariant MyTMSUUI_Config::getValue(const QString& configName)
{
   return getValue(configName, "");
}

QVariant MyTMSUUI_Config::getValue(const QString& configName, const QVariant& dfltValue)
{
   // Command Line Arg > env > conf file
   // (a.k.a.: QCommandLineParser > qgetenv > QSettings)

   // First, check command line for config setting...
   if (myCLArgParser.isSet(configName))
   {
      return QVariant(myCLArgParser.value(configName));
   }

   // Else, check environment next...

   QString envVarName(ENV_VAR_NAME_PREFIX);
   envVarName += configName;
   if (qEnvironmentVariableIsSet(envVarName.toStdString().c_str()))
   {
      return QVariant(qEnvironmentVariable(envVarName.toStdString().c_str()));
   }

   // Else, check config file...
   return mySettings.value(configName, dfltValue);
}


