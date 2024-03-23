#ifndef MYTMSUUI_MAINWINDOW_H
#define MYTMSUUI_MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
   class MyTMSUUI_MainWindow;
}
QT_END_NAMESPACE

// Forward declarations
class MyTMSUUI_Data;

// ============================================================================

class MyTMSUUI_MainWindow : public QMainWindow
{
 Q_OBJECT

 public:
   MyTMSUUI_MainWindow(QWidget* parent = nullptr);
   ~MyTMSUUI_MainWindow();

   void setDataObj(MyTMSUUI_Data* dataPtr);

 signals:
   void dataBaseDirChanged(const QString& p);

 protected slots:
   void doSelectBaseDir();
   void firstButtonClicked();
   void prevButtonClicked();
   void nextButtonClicked();
   void lastButtonClicked();
   void applyButtonClicked();
   void doUpdateRecurse(int newRecurseState);
   void radioClicked();
   void setStatusUpdating();

 private:
   Ui::MyTMSUUI_MainWindow* myGuiPtr;
   MyTMSUUI_Data* myDataPtr;
};

#endif // MYTMSUUI_MAINWINDOW_H
