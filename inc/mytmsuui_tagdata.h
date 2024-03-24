#ifndef MYTMSUUI_TAGDATA_H
#define MYTMSUUI_TAGDATA_H

#include <QSharedData>
#include <QString>
#include <QList>

// Declare the class prior to its "Share Data" class,
// which has list of pointers to the class.
// See THIS IS THE CLASS TO BE USED below.
class MyTMSUUI_TagData;

// Put the data members into a QSharedData-derived class
// to implement an "implicitly shared" (copy-on-write) class.
// This allows ease in usage within Qt container classes.
class MyTMSUUI_TagDataShared : public QSharedData
{
 public:
   // Default c'tor
   MyTMSUUI_TagDataShared()
   {
      // Does nothing (yet)
   }

   // Copy c'tor
   MyTMSUUI_TagDataShared(const MyTMSUUI_TagDataShared& other)
    : QSharedData(other)
    ,       tagName(other.tagName)
    ,    valuesList(other.valuesList)
    ,   impliesList(other.impliesList)
    , impliedByList(other.impliedByList)
   {
      // Does nothing but initialize (yet)
   }

   // Default d'tor
   ~MyTMSUUI_TagDataShared()
   {
      // Does nothing (yet)
   }

   // Data Members
   QString tagName;
   QList<QString> valuesList;
   QList<MyTMSUUI_TagData*> impliesList;
   QList<MyTMSUUI_TagData*> impliedByList;
};

// ==================================================
// ========== THIS IS THE CLASS TO BE USED ==========
// ==================================================
class MyTMSUUI_TagData
{
 public:
   // Default c'tor
   MyTMSUUI_TagData();

   // Explicit c'tor
   explicit MyTMSUUI_TagData(const QString& tagName);

   // Copy c'tor
   MyTMSUUI_TagData(const MyTMSUUI_TagData& other);

   // The default d'tor provided by the compiler is sufficient.

   // The implicit copy assignment operator provided by the compiler is sufficient.

   // Accessors
   QString getTagName() const;
   void setTagName(const QString& tagName);

   QList<QString> getValuesList() const;
   void addTagValue(const QString& value);

   QList<MyTMSUUI_TagData*> getImpliesList() const;
   void implies(MyTMSUUI_TagData* otherTag);

   QList<MyTMSUUI_TagData*> getImpliedByList() const;

 protected:
   void addImpliedBy(MyTMSUUI_TagData* otherTag); // Should only be called by implies

 private:
   QSharedDataPointer<MyTMSUUI_TagDataShared> my;
};

// =======
// INLINES
// =======

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

inline QList<MyTMSUUI_TagData*> MyTMSUUI_TagData::getImpliesList() const
{
	return my->impliesList;
}

inline QList<MyTMSUUI_TagData*> MyTMSUUI_TagData::getImpliedByList() const
{
	return my->impliedByList;
}

#endif
