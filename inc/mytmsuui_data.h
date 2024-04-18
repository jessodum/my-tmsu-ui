#ifndef MYTMSUUI_DATA_H
#define MYTMSUUI_DATA_H

#include "mytmsuui_interface.h"
#include "mytmsuui_tagdata.h"
#include "mytmsuui_implication.h"
#include <QDir>

class MyTMSUUI_Data
{
 public:
   MyTMSUUI_Data();
   ~MyTMSUUI_Data();

   //// Properities
   QDir myCurrentBaseDir;
   bool myRecurseEnabled;
   MyTMSUUI_IF_NS::EmptyQueryAction myEmptyQueryAction;
   QStringList myCurrentFilesList;
   qsizetype myCurrentImageNum;
   QList<MyTMSUUI_TaggedValue> myCurrentImgTaggedValList;
   MyTMSUUI_Interface myInterface;
   MyTMSUUI_TagDataPtrList myTagsList;
   QStringList myValuesList;
   QList<MyTMSUUI_Implication> myImplicationsList;

   void clearTagsList();
   QString getCurrentFilename();
   QString getCurrentFileFullPath();
   QList<MyTMSUUI_TaggedValue> getImpliedTaggedValuesList(const MyTMSUUI_TaggedValue& impliesTaggedValue) const;

 protected:

 private:

};

//// =======
//// INLINES
//// =======

inline QString MyTMSUUI_Data::getCurrentFilename()
{
   if ((myCurrentFilesList.size() > 0) && (myCurrentImageNum > 0))
   {
      return myCurrentFilesList.at(myCurrentImageNum - 1);
   }
   //// else

   return "";
}

inline QString MyTMSUUI_Data::getCurrentFileFullPath()
{
   QString filename = getCurrentFilename();
   if (!filename.isEmpty())
   {
      return (myCurrentBaseDir.absolutePath() + "/" + filename);
   }
   //// else

   return "";
}

#endif
