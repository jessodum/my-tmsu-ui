#ifndef MYTMSUUI_TAGGEDVALUE_H
#define MYTMSUUI_TAGGEDVALUE_H

#include <QString>

namespace MyTMSUUI_Tagged_NS
{
   enum CheckedState
   {
      Unchecked,
      SetExplicitTag,
      SetImpliedTag,
      SetBothTag,
      ToBeUnchecked,
      ToBeSetExplicitTag,
      ToBeSetImpliedTag,
      ToBeSetBothTag
   };
}

struct MyTMSUUI_TaggedValue
{
   QString myTagName;
   QString myValue;
   MyTMSUUI_Tagged_NS::CheckedState myCheckedState;

   MyTMSUUI_TaggedValue()
    : myTagName("")
    , myValue("")
    , myCheckedState(MyTMSUUI_Tagged_NS::Unchecked)
   {
      //// Does nothing but initialize (yet)
   }

   // Default d'tor
   ~MyTMSUUI_TaggedValue()
   {
      //// Does nothing (yet)
   }

   bool operator==(const MyTMSUUI_TaggedValue& other) const;
   bool operator!=(const MyTMSUUI_TaggedValue& other) const;
   bool operator==(const QString& otherTagName) const;
   bool operator!=(const QString& otherTagName) const;
   friend bool operator== (const QString& otherTagName, const MyTMSUUI_TaggedValue& me);
   friend bool operator!= (const QString& otherTagName, const MyTMSUUI_TaggedValue& me);
};

//// =======
//// INLINES
//// =======

inline bool MyTMSUUI_TaggedValue::operator==(const MyTMSUUI_TaggedValue& other) const
{
   return myTagName == other.myTagName;
}

inline bool MyTMSUUI_TaggedValue::operator!=(const MyTMSUUI_TaggedValue& other) const
{
   return myTagName != other.myTagName;
}

inline bool MyTMSUUI_TaggedValue::operator==(const QString& otherTagName) const
{
   return myTagName == otherTagName;
}

inline bool MyTMSUUI_TaggedValue::operator!=(const QString& otherTagName) const
{
   return myTagName != otherTagName;
}

inline bool operator== (const QString& otherTagName, const MyTMSUUI_TaggedValue& me)
{
   return me == otherTagName;
}

inline bool operator!= (const QString& otherTagName, const MyTMSUUI_TaggedValue& me)
{
   return me != otherTagName;
}

#endif //// MYTMSUUI_TAGGEDVALUE_H
