#ifndef MYTMSUUI_TAGDATA_H
#define MYTMSUUI_TAGDATA_H

#include <QSharedData>
#include <QString>
#include <QList>

//// Declare the class prior to its "Share Data" class,
//// which has list of pointers to the class.
//// See THIS IS THE CLASS TO BE USED below.
class MyTMSUUI_TagData;

// Type aliases
typedef QList<MyTMSUUI_TagData> MyTMSUUI_TagDataList;
typedef QList<MyTMSUUI_TagData*> MyTMSUUI_TagDataPtrList;

//// Put the data members into a QSharedData-derived class
//// to implement an "implicitly shared" (copy-on-write) class.
//// This allows ease in usage within Qt container classes.
class MyTMSUUI_TagDataShared : public QSharedData
{
 public:
   //// Default c'tor
   MyTMSUUI_TagDataShared()
   {
      //// Does nothing (yet)
   }

   //// Copy c'tor
   MyTMSUUI_TagDataShared(const MyTMSUUI_TagDataShared& other)
    : QSharedData(other)
    ,       tagName(other.tagName)
    ,    valuesList(other.valuesList)
    ,   impliesList(other.impliesList)
    , impliedByList(other.impliedByList)
   {
      //// Does nothing but initialize (yet)
   }

   // Default d'tor
   ~MyTMSUUI_TagDataShared()
   {
      //// Does nothing (yet)
   }

   //// Data Members
   QString tagName;
   QList<QString> valuesList;
   MyTMSUUI_TagDataPtrList impliesList;
   MyTMSUUI_TagDataPtrList impliedByList;
};

//// ==================================================
//// ========== THIS IS THE CLASS TO BE USED ==========
//// ==================================================
class MyTMSUUI_TagData
{
 public:
   //// Default c'tor
   MyTMSUUI_TagData();

   //// Explicit c'tor
   explicit MyTMSUUI_TagData(const QString& tagName);

   //// Copy c'tor
   MyTMSUUI_TagData(const MyTMSUUI_TagData& other);

   //// The default d'tor provided by the compiler is sufficient.

   //// The implicit copy assignment operator provided by the compiler is sufficient.

   //// Equality operator overload
   bool operator==(const MyTMSUUI_TagData& other) const;

   //// Static utilities for QList<MyTMSUUI_TagData*>
   static bool listOfPointersContains(const MyTMSUUI_TagDataPtrList& list, const QString& tagName);
   static bool listOfPointersContains(const MyTMSUUI_TagDataPtrList& list, const MyTMSUUI_TagData& tagObj);
   static bool listOfPointersContains(const MyTMSUUI_TagDataPtrList& list, MyTMSUUI_TagData* tagPtr);
   static MyTMSUUI_TagData* findInListOfPointers(const MyTMSUUI_TagDataPtrList& list, const QString& tagName);
   static MyTMSUUI_TagData* findInListOfPointers(const MyTMSUUI_TagDataPtrList& list, const MyTMSUUI_TagData& tagObj);
   static MyTMSUUI_TagData* findInListOfPointers(const MyTMSUUI_TagDataPtrList& list, MyTMSUUI_TagData* tagPtr);

   //// Accessors
   QString getTagName() const;
   void setTagName(const QString& tagName);

   QList<QString> getValuesList() const;
   void addTagValue(const QString& value);

   MyTMSUUI_TagDataPtrList getImpliesList() const;
   void implies(MyTMSUUI_TagData* otherTag);

   MyTMSUUI_TagDataPtrList getImpliedByList() const;

 protected:
   void addImpliedBy(MyTMSUUI_TagData* otherTag); //// Should only be called by implies

 private:
   QSharedDataPointer<MyTMSUUI_TagDataShared> my;
};

//// =======
//// INLINES
//// =======

inline bool MyTMSUUI_TagData::operator==(const MyTMSUUI_TagData& other) const
{
   return (getTagName() == other.getTagName());
}

inline bool MyTMSUUI_TagData::listOfPointersContains(const MyTMSUUI_TagDataPtrList& list, const MyTMSUUI_TagData& tagObj)
{
   return MyTMSUUI_TagData::listOfPointersContains(list, tagObj.getTagName());
}

inline bool MyTMSUUI_TagData::listOfPointersContains(const MyTMSUUI_TagDataPtrList& list, MyTMSUUI_TagData* tagPtr)
{
   if (tagPtr == nullptr)
   {
      return false;
   }
   else
   {
      return MyTMSUUI_TagData::listOfPointersContains(list, *tagPtr);
   }
}

inline MyTMSUUI_TagData* MyTMSUUI_TagData::findInListOfPointers(const MyTMSUUI_TagDataPtrList& list, const MyTMSUUI_TagData& tagObj)
{
   return MyTMSUUI_TagData::findInListOfPointers(list, tagObj.getTagName());
}

inline MyTMSUUI_TagData* MyTMSUUI_TagData::findInListOfPointers(const MyTMSUUI_TagDataPtrList& list, MyTMSUUI_TagData* tagPtr)
{
   if (tagPtr == nullptr)
   {
      return nullptr;
   }
   else
   {
      return MyTMSUUI_TagData::findInListOfPointers(list, *tagPtr);
   }
}

inline QString MyTMSUUI_TagData::getTagName() const
{
   return my->tagName;
}

inline void MyTMSUUI_TagData::setTagName(const QString& tagName)
{
   my->tagName = tagName;
}

inline QList<QString> MyTMSUUI_TagData::getValuesList() const
{
   return my->valuesList;
}

inline MyTMSUUI_TagDataPtrList MyTMSUUI_TagData::getImpliesList() const
{
	return my->impliesList;
}

inline MyTMSUUI_TagDataPtrList MyTMSUUI_TagData::getImpliedByList() const
{
	return my->impliedByList;
}

#endif