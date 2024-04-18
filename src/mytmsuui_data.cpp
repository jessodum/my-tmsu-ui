#include "mytmsuui_data.h"

//// --------------------------------------------------------------------------
MyTMSUUI_Data::MyTMSUUI_Data()
 : myCurrentBaseDir()
 , myRecurseEnabled(false)
 , myEmptyQueryAction(MyTMSUUI_IF_NS::RetrieveNone)
 , myCurrentFilesList()
 , myCurrentImageNum(0)
 , myCurrentImgTaggedValList()
 , myInterface()
 , myTagsList()
 , myValuesList()
 , myImplicationsList()
{
   myInterface.setDataObj(this);
}

//// --------------------------------------------------------------------------
MyTMSUUI_Data::~MyTMSUUI_Data()
{
   //// Nothing to do here (yet)
}

//// --------------------------------------------------------------------------
void MyTMSUUI_Data::clearTagsList()
{
   // This function is used instead of just using QList::clear() because we
   // want to ensure that the dynamically allocated TagData objects are
   // also deleted (to prevent memory leaks).

   while (!myTagsList.isEmpty())
   {
      MyTMSUUI_TagData* ptr = myTagsList.takeLast();
      if (ptr != nullptr)
      {
         delete ptr;
      }
   }
}

//// --------------------------------------------------------------------------
QList<MyTMSUUI_TaggedValue> MyTMSUUI_Data::getImpliedTaggedValuesList(const MyTMSUUI_TaggedValue& impliesTaggedValue) const
{
   QList<MyTMSUUI_TaggedValue> retList;

   for (MyTMSUUI_Implication implc : myImplicationsList)
   {
      if (implc.myImpliesTaggedValue == impliesTaggedValue)
      {
         retList.append(implc.myImpliedTaggedValue);
      }
   }

   return retList;
}
