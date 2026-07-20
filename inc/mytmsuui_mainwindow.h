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

   //// Description: Set accessor for the pointer to the application's Data object
   void setDataObj(MyTMSUUI_Data* dataPtr);

 signals:
   void dataBaseDirChanged(const QString& p);
   void imageUpdated(const QString& f);

 protected:

   //// Description: Adds/removes/updates clone of main tag widget in the "short list" of tag widgets.
   void applyTagWidgetToShortList(MyTMSUUI_TagWidget* tagWidgetPtr, const MyTMSUUI_MainWin_NS::ShortListModAction action);

   //// Description: Resets GUI for a new list of images.
   void beginDisplayList(bool emptyListIsOK = false);

   //// Description: Recursively builds list of implied tags from in the input TaggedValue
   void buildImpliedTagChainsList(QList<MyTMSUUI_TaggedValue>* listToBuild, const MyTMSUUI_TaggedValue& impliesTaggedValue);

   //// Description: Looks for any "unapplied" (i.e., ToBe*) checkbox states and,
   ////    if found, presents user with "Unapplied Tags" dialog box to determine
   ////    course of action.
   MyTMSUUI_MainWin_NS::CheckUnAppliedResult checkForUnappliedTags(bool canCancelAction = true, bool fromQueryClick = false);

   //// Description: Removes all TagWidgets from main list
   void clearTagWidgets();

   //// Description: Event handler for closing the GUI
   virtual void closeEvent(QCloseEvent* event);

   //// Description: Find a "clone" TagWidget from the "short list" that matches the input TagWidget
   MyTMSUUI_TagWidget* findCloneTagWidget(MyTMSUUI_TagWidget* origTagWidget);

   //// Description: Find a TagWidget from the "main list" that matches the input tag name
   MyTMSUUI_TagWidget* findTagWidget(const QString& tagName);

   //// Description: Returns list of TagWidgets for either "main" or "short" list
   TagWidgetList getTagWidgetList(bool useShortList = false);

   //// Description: Jump to an image in the list by its number in the list
   void goToImage(qsizetype number);

   //// Description: Jump to the last image in the list
   void goToLastImage();

   //// Description: Returns flag indicating whether the current image is an animation
   bool isCurrentImageAnim();

   //// Description: Iterate through the temporary list of "image" files to filter out non-image files by format inspection
   void prepFilesListForDisplay();

   //// Description: Rebuilds the "main" list of TagWidgets
   void rebuildTagWidgets();

   //// Description: Updates the enabled states for the navigation buttons
   void setNavEnabledStates();

   //// Description: Updates the image numbers in the navigation area: current (entry) and max (label)
   void setNavImgNumTexts(qsizetype currNum, qsizetype maxNum);

   //// Description: Updates the TagWidgets (in both "main" and "short" lists) to match the TaggedValues of the current image
   void setTaggedValuesInWidgets();

   //// Description: Unchecks all TagWidgets in "main" list; removes clones from "short" list
   void uncheckAllTagWidgets();

   //// Description: Requests that the sub-process interface update the list of files
   void updateInterfaceFilesList();

   //// Description: Updates the list of query tags in the sub-process interface
   void updateInterfaceQueryTagsList();

   //// Description: Display the current image and update other GUI elements as applicable
   void updateUiForCurrentImage();

 protected slots:

   //// Description: callback for clicking the "Apply" button
   void applyButtonClicked();

   //// Description: Displays "About" dialog
   void doAbout();

   //// Description: Opens "User Manual" / help doc web page
   void doOpenUserManual();

   //// Description: Display dir selection dialog and act upon user's input
   void doSelectBaseDir();

   //// Description: Callback for the "Recurse into subdirectories" checkbox
   void doUpdateRecurse(bool newRecurseState);

   //// Description: Callback for the "Jump to 1st image" button
   void firstButtonClicked();

   //// Description: Callback for a "main" TagWidget checkbox clicked
   void handleMainTagToggled(const QString& tagName, bool byUserClick);

   //// Description: Callback for the index of a "main" TagWidget value combobox changing
   void handleMainTagValIdxChanged(const QString& tagName, int index);

   //// Description: Callback for a "short list" TagWidget checkbox clicked
   void handleShortTagClicked(const QString& tagName, bool byUserClick);

   //// Description: Callback for the index of a "short list" TagWidget value combobox changing
   void handleShortTagValIdxChanged(const QString& tagName, int index);

   //// Description: Callback for sub-process interface completing work
   void interfaceGoneIdle(MyTMSUUI_IF_NS::ProcState lastState,
                          MyTMSUUI_IF_NS::LastStateErrorCode errorCode = MyTMSUUI_IF_NS::EC_NoError);

   //// Description: Callback for changing the image number manually by the entry field
   void jumpToImageFromEntry();

   //// Description: Callback for the "Jump to last image" button
   void lastButtonClicked();

   //// Description: Callback for the "Go to next image" button
   void nextButtonClicked();

   //// Description: Callback for the "Go to previous image" button
   void prevButtonClicked();

   //// Description: Callback for clicking the (retrieve) "All" radio button
   void radioAllClicked();

   //// Description: Callback for clicking the (retrieve) "None" radio button
   void radioNoneClicked();

   //// Description: Callback for clicking the "Query" radio button
   void radioQueryClicked();

   //// Description: Callback for clicking the "Set" radio button
   void radioSetTagsClicked();

   //// Description: Callback for clicking the (retrieve) "Untagged" radio button
   void radioUntaggedClicked();

   //// Description: Callback for clicking the "Reset" button
   void resetButtonClicked();

   //// Description: Callback for clicking the "Scroll to bottom" button
   void scrollToBottomClicked();

   //// Description: Callback for clicking the "Scroll to top" button
   void scrollToTopClicked();

   //// Description: Displays "Updating..." in the status bar
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
