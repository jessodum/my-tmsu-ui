#include "mytmsuui_config.h"
#include "mytmsuui_consts.h"
#include <QCoreApplication>

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
   myCLArgParser.setApplicationDescription(MyTMSUUI_Consts::APP_DESCRIPTION);
   myCLArgParser.addHelpOption();
   myCLArgParser.addVersionOption();
// TODO: Set up the parser
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Config::setApp(QCoreApplication* appPtr)
{
   static bool setOnce = false;

   if (setOnce)
   {
      qWarning("App has already been set for config");
      return;
   }

   if (appPtr == nullptr)
   {
      qWarning("NULL app pointer provided for config");
      return;
   }

   myCLArgParser.process(*appPtr);
   setOnce = true;
}

