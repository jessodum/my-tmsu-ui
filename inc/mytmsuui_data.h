#ifndef MYTMSUUI_DATA_H
#define MYTMSUUI_DATA_H

#include "mytmsuui_interface.h"
#include "mytmsuui_tagdata.h"
#include <QDir>
#include <QList>

class MyTMSUUI_Data
{
 public:
   MyTMSUUI_Data();
   ~MyTMSUUI_Data();

   //// Properities
   QDir myCurrentBaseDir;
   bool myRecurseEnabled;
   MyTMSUUI_Interface myInterface;
   MyTMSUUI_TagDataPtrList myTagsList;
   QList<QString> myValuesList;

   void clearTagsList();

 protected:

 private:

};

#endif
