#ifndef MYTMSUUI_MAINWINDOW_H
#define MYTMSUUI_MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
   class MyTMSUUI_MainWindow;
}
QT_END_NAMESPACE

// ============================================================================

class MyTMSUUI_MainWindow : public QMainWindow
{
 Q_OBJECT

 public:
   MyTMSUUI_MainWindow(QWidget* parent = nullptr);
   ~MyTMSUUI_MainWindow();

 protected slots:
   void doSelectBaseDir();
   void firstButtonClicked();
   void prevButtonClicked();
   void nextButtonClicked();
   void lastButtonClicked();
   void applyButtonClicked();
   void doUpdateRecurse(int newRecurseState);
   void radioClicked();

 private:
   Ui::MyTMSUUI_MainWindow* myGuiPtr;
};

#endif // MYTMSUUI_MAINWINDOW_H
