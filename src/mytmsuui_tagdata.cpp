#include "mytmsuui_tagdata.h"

MyTMSUUI_TagData::MyTMSUUI_TagData()
{
   my = new MyTMSUUI_TagDataShared();
}

MyTMSUUI_TagData::MyTMSUUI_TagData(const QString& tagName)
{
   my = new MyTMSUUI_TagDataShared();
   setTagName(tagName);
}

MyTMSUUI_TagData::MyTMSUUI_TagData(const MyTMSUUI_TagData& other)
 : my (other.my)
{
   // Does nothing more than copy via initializer (yet)
}

void MyTMSUUI_TagData::addTagValue(const QString& value)
{
   // TODO
}

void MyTMSUUI_TagData::implies(MyTMSUUI_TagData* otherTag)
{
   // TODO
}

void MyTMSUUI_TagData::addImpliedBy(MyTMSUUI_TagData* otherTag)
{
   // (Should only be called by implies)
   // TODO
}

