#ifndef MYTMSUUI_IMPLICATION_H
#define MYTMSUUI_IMPLICATION_H

#include "mytmsuui_taggedvalue.h"

struct MyTMSUUI_Implication
{
   MyTMSUUI_TaggedValue myImpliesTaggedValue;
   MyTMSUUI_TaggedValue myImpliedTaggedValue;

   bool operator==(const MyTMSUUI_Implication& other) const;
   bool operator!=(const MyTMSUUI_Implication& other) const;
};

inline bool MyTMSUUI_Implication::operator== (const MyTMSUUI_Implication& other) const
{
   return ( myImpliesTaggedValue == other.myImpliesTaggedValue 
         && myImpliedTaggedValue == other.myImpliedTaggedValue );
}

inline bool MyTMSUUI_Implication::operator!= (const MyTMSUUI_Implication& other) const
{
   return !(*this == other);
}

#endif
