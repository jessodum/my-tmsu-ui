#include "mytmsuui_data.h"

MyTMSUUI_Data::MyTMSUUI_Data()
 : myCurrentBaseDir()
 , myInterface()
{
   myInterface.setDataObj(this);
}

MyTMSUUI_Data::~MyTMSUUI_Data()
{
   // Nothing to do here (yet)
}
