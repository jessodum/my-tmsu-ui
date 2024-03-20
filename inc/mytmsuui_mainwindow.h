#ifndef MYTMSUUI_MAINWINDOW_H
#define MYTMSUUI_MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
   class MyTMSUUI_MainWindow;
}
QT_END_NAMESPACE

class MyTMSUUI_MainWindow : public QMainWindow
{
 Q_OBJECT

 public:
   MyTMSUUI_MainWindow(QWidget* parent = nullptr);
   ~MyTMSUUI_MainWindow();

 private:
   Ui::MyTMSUUI_MainWindow* ui;
};

#endif // MYTMSUUI_MAINWINDOW_H
