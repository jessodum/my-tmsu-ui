#include "mytmsuui_interface.h"
#include <stdio.h> // TODO: remove

// ----------------------------------------------------------------------------
MyTMSUUI_Interface::MyTMSUUI_Interface(QObject* parent)
 : QObject(parent)
 , myIFProc()
{
   // Nothing to do here (yet)
}

// ----------------------------------------------------------------------------
MyTMSUUI_Interface::~MyTMSUUI_Interface()
{
   // Nothing to do here (yet)
}

// ----------------------------------------------------------------------------
void MyTMSUUI_Interface::doNewBaseDir(const QString& newPath) // TODO
{
   printf("TODO doNewBaseDir %s\n", newPath.toStdString().c_str());
   return;
}
