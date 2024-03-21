#ifndef MYTMSUUI_DATA_H
#define MYTMSUUI_DATA_H

#include <QDir>

class MyTMSUUI_Data
{
 public:
   MyTMSUUI_Data();
   ~MyTMSUUI_Data();

   QDir getCurrentBaseDir() const;

 protected:

 private:
   QDir myCurrentBaseDir;
};

inline QDir MyTMSUUI_Data::getCurrentBaseDir() const
{
   return myCurrentBaseDir;
}

#endif
