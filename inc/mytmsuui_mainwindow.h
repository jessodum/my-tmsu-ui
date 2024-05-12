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

   enum ShortListModAction
   {
      SL_MOD_NO_ACTION,
      SL_MOD_ADD,
      SL_MOD_REMOVE,
      SL_MOD_UPDATE
   };

   enum CheckUnAppliedResult
   {
      CK_UA_CONTINUE,
      CK_UA_CANCEL
   };
}

//// Forward declarations
class QLabel;
class MyTMSUUI_TagWidget;

//// Typedefs (type aliases)
typedef QList<MyTMSUUI_TagWidget*> TagWidgetList;

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
   void applyTagWidgetToShortList(MyTMSUUI_TagWidget* tagWidgetPtr, const MyTMSUUI_MainWin_NS::ShortListModAction action);
   void beginDisplayList(bool emptyListIsOK = false);
   void buildImpliedTagChainsList(QList<MyTMSUUI_TaggedValue>* listToBuild, const MyTMSUUI_TaggedValue& impliesTaggedValue);
   MyTMSUUI_MainWin_NS::CheckUnAppliedResult checkForUnappliedTags(bool canCancelAction = true, bool fromQueryClick = false);
   void clearTagWidgets();
   virtual void closeEvent(QCloseEvent* event);
   MyTMSUUI_TagWidget* findCloneTagWidget(MyTMSUUI_TagWidget* origTagWidget);
   MyTMSUUI_TagWidget* findTagWidget(const QString& tagName);
   TagWidgetList getTagWidgetList(bool useShortList = false);
   void goToImage(qsizetype number);
   void goToLastImage();
   bool isCurrentImageAnim();
   void prepFilesListForDisplay();
   void rebuildTagWidgets();
   void setNavEnabledStates();
   void setNavImgNumTexts(qsizetype currNum, qsizetype maxNum);
   void setTaggedValuesInWidgets();
   void uncheckAllTagWidgets();
   void updateInterfaceFilesList();
   void updateInterfaceQueryTagsList();
   void updateUiForCurrentImage();

 protected slots:
   void applyButtonClicked();
   void doAbout();
   void doOpenUserManual();
   void doSelectBaseDir();
   void doUpdateRecurse(bool newRecurseState);
   void firstButtonClicked();
   void handleMainTagToggled(const QString& tagName, bool byUserClick);
   void handleMainTagValIdxChanged(const QString& tagName, int index);
   void handleShortTagClicked(const QString& tagName, bool byUserClick);
   void handleShortTagValIdxChanged(const QString& tagName, int index);
   void interfaceGoneIdle(MyTMSUUI_IF_NS::ProcState lastState, bool withError = false);
   void jumpToImageFromEntry();
   void lastButtonClicked();
   void nextButtonClicked();
   void prevButtonClicked();
   void radioAllClicked();
   void radioNoneClicked();
   void radioQueryClicked();
   void radioSetTagsClicked();
   void radioUntaggedClicked();
   void resetButtonClicked();
   void scrollToBottomClicked();
   void scrollToTopClicked();
   void setStatusUpdating();

 private:
   Ui::MyTMSUUI_MainWindow* myGuiPtr;
   QLabel* myGuiStatusBarNormalLabel;
   QLabel* myGuiStatusBarErrorLabel;
   MyTMSUUI_Data* myDataPtr;
   bool myToggleOtherTagsAllowed;
   size_t myShortListWidgetNum;
};

#endif //// MYTMSUUI_MAINWINDOW_H
