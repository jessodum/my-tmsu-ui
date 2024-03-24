#include "mytmsuui_tagdata.h"

// ----------------------------------------------------------------------------
MyTMSUUI_TagData::MyTMSUUI_TagData()
{
   my = new MyTMSUUI_TagDataShared();
}

// ----------------------------------------------------------------------------
MyTMSUUI_TagData::MyTMSUUI_TagData(const QString& tagName)
{
   my = new MyTMSUUI_TagDataShared();
   setTagName(tagName);
}

// ----------------------------------------------------------------------------
MyTMSUUI_TagData::MyTMSUUI_TagData(const MyTMSUUI_TagData& other)
 : my (other.my)
{
   // Does nothing more than copy via initializer (yet)
}

// ----------------------------------------------------------------------------
bool MyTMSUUI_TagData::listOfPointersContains(const MyTMSUUI_TagDataPtrList& list, const QString& tagName)
{
   bool retval = false;

   for (MyTMSUUI_TagData* entry : list)
   {
      if (entry->getTagName() == tagName)
      {
         retval = true;
         break;
      }
   }

   return retval;
}

// ----------------------------------------------------------------------------
MyTMSUUI_TagData* MyTMSUUI_TagData::findInListOfPointers(const MyTMSUUI_TagDataPtrList& list, const QString& tagName)
{
   MyTMSUUI_TagData* retval = nullptr;

   for (MyTMSUUI_TagData* entry : list)
   {
      if (entry->getTagName() == tagName)
      {
         retval = entry;
         break;
      }
   }

   return retval;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_TagData::addTagValue(const QString& value)
{
   if (! my->valuesList.contains(value) )
   {
      my->valuesList.append(value);
   }
}

// ----------------------------------------------------------------------------
void MyTMSUUI_TagData::implies(MyTMSUUI_TagData* otherTag)
{
   if (! listOfPointersContains(my->impliesList, otherTag) )
   {
      my->impliesList.append(otherTag);
   }

   if (! listOfPointersContains(otherTag->getImpliedByList(), this) )
   {
      otherTag->addImpliedBy(this);
   }
}

// ----------------------------------------------------------------------------
void MyTMSUUI_TagData::addImpliedBy(MyTMSUUI_TagData* otherTag)
{
   // (Should only be called by implies)
   if (! listOfPointersContains(my->impliedByList, otherTag) )
   {
      my->impliedByList.append(otherTag);
   }
}

