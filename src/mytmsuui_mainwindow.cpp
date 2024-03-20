#include "mytmsuui_mainwindow.h"
#include "ui_mytmsuui_mainwindow.h"

MyTMSUUI_MainWindow::MyTMSUUI_MainWindow(QWidget* parent)
 : QMainWindow(parent)
 , ui(new Ui::MyTMSUUI_MainWindow)
{
   ui->setupUi(this);
}

MyTMSUUI_MainWindow::~MyTMSUUI_MainWindow()
{
   delete ui;
}
