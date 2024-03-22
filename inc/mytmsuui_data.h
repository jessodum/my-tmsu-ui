#ifndef MYTMSUUI_DATA_H
#define MYTMSUUI_DATA_H

#include "mytmsuui_interface.h"
#include <QDir>

class MyTMSUUI_Data
{
 public:
   MyTMSUUI_Data();
   ~MyTMSUUI_Data();

   // Properities
   QDir myCurrentBaseDir;
   MyTMSUUI_Interface myInterface;

 protected:

 private:

};

#endif
