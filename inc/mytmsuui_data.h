#ifndef MYTMSUUI_DATA_H
#define MYTMSUUI_DATA_H

#include "mytmsuui_interface.h"
#include "mytmsuui_tagdata.h"
#include <QDir>

class MyTMSUUI_Data
{
 public:
   MyTMSUUI_Data();
   ~MyTMSUUI_Data();

   //// Properities
   QDir myCurrentBaseDir;
   QStringList myCurrentFilesList;
   qsizetype myCurrentImageNum;
   bool myRecurseEnabled;
   MyTMSUUI_Interface myInterface;
   MyTMSUUI_TagDataPtrList myTagsList;
   QStringList myValuesList;

   void clearTagsList();

 protected:

 private:

};

#endif
