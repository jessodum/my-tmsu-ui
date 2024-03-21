#include "mytmsuui_mainwindow.h"
#include "mytmsuui_data.h"

#include <QApplication>

int main(int argc, char* argv[])
{
   QApplication theApp(argc, argv);
   MyTMSUUI_MainWindow theMainWin;
   MyTMSUUI_Data dataObj;

   theMainWin.setDataObj(&dataObj);

   // Display the Main Window
   theMainWin.show();

   // Run the GUI (Application)
   return theApp.exec();
}
