#include "mytmsuui_mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
   QApplication theApp(argc, argv);
   MyTMSUUI_MainWindow theMainWin;

   // Display the Main Window
   theMainWin.show();

   // Run the GUI (Application)
   return theApp.exec();
}
