#ifndef MYTMSUUI_MAINWINDOW_H
#define MYTMSUUI_MAINWINDOW_H

#include <QMainWindow>
#include "mytmsuui_data.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
   class MyTMSUUI_MainWindow;
}
QT_END_NAMESPACE

namespace MyTMSUUI_MainWin_NS
{
   static const int MAX_IMAGE_HEIGHT = 1140;
}

//// Forward declarations
class QLabel;

//// ==========================================================================

class MyTMSUUI_MainWindow : public QMainWindow
{
 Q_OBJECT

 public:
   MyTMSUUI_MainWindow(QWidget* parent = nullptr);
   ~MyTMSUUI_MainWindow();

   void setDataObj(MyTMSUUI_Data* dataPtr);

 signals:
   void dataBaseDirChanged(const QString& p);
   void imageUpdated(const QString& f);

 protected:
   virtual void closeEvent(QCloseEvent* event);
   void clearTagWidgets();
   void rebuildTagWidgets();
   void prepFilesListForDisplay();
   void beginDisplayList();
   void goToImage(qsizetype number);
   void goToLastImage();
   void updateUiForCurrentImage();
   void setNavEnabledStates();
   bool isCurrentImageAnim();

 protected slots:
   void doSelectBaseDir();
   void firstButtonClicked();
   void prevButtonClicked();
   void nextButtonClicked();
   void lastButtonClicked();
   void applyButtonClicked();
   void doUpdateRecurse(int newRecurseState);
   void radioQueryClicked();
   void radioSetTagsClicked();
   void radioAllClicked();
   void radioNoneClicked();
   void setStatusUpdating();
   void interfaceGoneIdle(MyTMSUUI_IF_NS::ProcState lastState, bool withError = false);

 private:
   Ui::MyTMSUUI_MainWindow* myGuiPtr;
   QLabel* myGuiStatusBarNormalLabel;
   QLabel* myGuiStatusBarErrorLabel;
   MyTMSUUI_Data* myDataPtr;
};

#endif //// MYTMSUUI_MAINWINDOW_H
