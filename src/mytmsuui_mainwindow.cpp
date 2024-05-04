#include "mytmsuui_mainwindow.h"
#include "ui_mytmsuui_mainwindow.h"
#include "mytmsuui_tagwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QImageReader>
#include <QMovie>
#include <QtLogging>

#define ENSURE_DATA_PTR_COMMON(whynot) \
   if (myDataPtr == nullptr) \
   { \
      qCritical("null data object (how did this happen?)"); \
      QString statusBarMsg("INTERNAL ERROR: Missing data object - "); \
      statusBarMsg += whynot; \
      statusBar()->showMessage(statusBarMsg);

#define ENSURE_DATA_PTR(whynot) \
ENSURE_DATA_PTR_COMMON(whynot) \
      return; \
   }

#define ENSURE_DATA_PTR_RETURN_FALSE(whynot) \
ENSURE_DATA_PTR_COMMON(whynot) \
      return false; \
   }

//// --------------------------------------------------------------------------
//// ctor (public)
//// --------------------------------------------------------------------------
MyTMSUUI_MainWindow::MyTMSUUI_MainWindow(QWidget* parent)
 : QMainWindow(parent)
 , myGuiPtr(new Ui::MyTMSUUI_MainWindow)
 , myGuiStatusBarNormalLabel(nullptr)
 , myGuiStatusBarErrorLabel(nullptr)
 , myDataPtr(nullptr)
 , myToggleOtherTagsAllowed(true)
 , myShortListWidgetNum(0)
{
   //// Setup the UI widgets (based on the Designer "ui" file)
   myGuiPtr->setupUi(this);

   //// Add ("normal") labels to StatusBar
   myGuiStatusBarNormalLabel = new QLabel(myGuiPtr->myStatusBar);
   myGuiStatusBarNormalLabel->setObjectName("myGuiStatusBarNormalLabel");
   myGuiStatusBarNormalLabel->setStyleSheet(QString::fromUtf8("border-right: 1px ridge; border-radius: 0px;"));

   myGuiStatusBarErrorLabel = new QLabel(myGuiPtr->myStatusBar);
   myGuiStatusBarErrorLabel->setObjectName("myGuiStatusBarErrorLabel");
   myGuiStatusBarErrorLabel->setStyleSheet(QString::fromUtf8("color: red;"));

   myGuiPtr->myStatusBar->addWidget(myGuiStatusBarNormalLabel);
   myGuiPtr->myStatusBar->addWidget(myGuiStatusBarErrorLabel);

   //// -----------------
   //// Setup Connections
   //// -----------------
   { //// Block for folding

   //// Action: Quit
   connect(myGuiPtr->myQuitAction, SIGNAL(triggered()),
                             this,   SLOT(    close()) );

   //// Action: Select base dir
   connect(myGuiPtr->mySelectBaseDirAction, SIGNAL(      triggered()),
                                      this,   SLOT(doSelectBaseDir()) );

   //// Action: Open help manual
   connect(myGuiPtr->myHelpManualAction, SIGNAL(       triggered()),
                                   this,   SLOT(doOpenUserManual()) );

   //// Action: About
   connect(myGuiPtr->myAboutAction, SIGNAL(triggered()),
                              this,   SLOT(  doAbout()) );

   //// Push Button: Select base dir
   connect(myGuiPtr->mySelectBaseDirBtn, SIGNAL(        clicked()),
                                   this,   SLOT(doSelectBaseDir()) );

   //// Push Button: First
   connect(myGuiPtr->myFirstImgButton, SIGNAL(           clicked()),
                                 this,   SLOT(firstButtonClicked()) );

   //// Push Button: Prev
   connect(myGuiPtr->myPrevImgButton, SIGNAL(          clicked()),
                                this,   SLOT(prevButtonClicked()) );

   //// Push Button: Next
   connect(myGuiPtr->myNextImgButton, SIGNAL(          clicked()),
                                this,   SLOT(nextButtonClicked()) );

   //// Push Button: Last
   connect(myGuiPtr->myLastImgButton, SIGNAL(          clicked()),
                                this,   SLOT(lastButtonClicked()) );

   //// Push Button: Apply
   connect(myGuiPtr->myApplyButton, SIGNAL(           clicked()),
                              this,   SLOT(applyButtonClicked()) );

   //// Push Button: Reset
   connect(myGuiPtr->myResetButton, SIGNAL(           clicked()),
                              this,   SLOT(resetButtonClicked()) );

   //// Push Button: Scroll To Bottom
   connect(myGuiPtr->myScrollToBottomButton, SIGNAL(              clicked()),
                                       this,   SLOT(scrollToBottomClicked()) );

   //// Push Button: Scroll To Top
   connect(myGuiPtr->myScrollToTopButton, SIGNAL(           clicked()),
                                    this,   SLOT(scrollToTopClicked()) );

   //// Check Box: Recurse
   connect(myGuiPtr->myRecurseCheckbox, SIGNAL(        toggled(bool)),
                                  this,   SLOT(doUpdateRecurse(bool)) );

   //// Radio Button: Query
   connect(myGuiPtr->myQueryRadioButton, SIGNAL(          clicked()),
                                   this,   SLOT(radioQueryClicked()) );

   //// Radio Button: Set tags
   connect(myGuiPtr->mySetTagsRadioButton, SIGNAL(            clicked()),
                                     this,   SLOT(radioSetTagsClicked()) );

   //// Radio Button: (Retrieve) Untagged
   connect(myGuiPtr->myRetrieveUntaggedRadioButton, SIGNAL(             clicked()),
                                              this,   SLOT(radioUntaggedClicked()) );

   //// Radio Button: (Retrieve) All
   connect(myGuiPtr->myRetrieveAllRadioButton, SIGNAL(        clicked()),
                                         this,   SLOT(radioAllClicked()) );

   //// Radio Button: (Retrieve) None
   connect(myGuiPtr->myRetrieveNoneRadioButton, SIGNAL(         clicked()),
                                          this,   SLOT(radioNoneClicked()) );

   //// this: Base dir changed
   connect(this, SIGNAL(dataBaseDirChanged(const QString&)),
           this,   SLOT( setStatusUpdating()) );
   connect(                            this, SIGNAL(dataBaseDirChanged(const QString&)),
           myGuiPtr->mySelectedBaseDirLabel,   SLOT(           setText(const QString&)) );

   } //// Block for folding

}

//// --------------------------------------------------------------------------
//// dtor (public)
//// --------------------------------------------------------------------------
MyTMSUUI_MainWindow::~MyTMSUUI_MainWindow()
{
   delete myGuiPtr;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::applyButtonClicked()
{
   ENSURE_DATA_PTR("cannot access interface object")

   //// Which mode? Query or Set?
   if (myGuiPtr->myQueryRadioButton->isChecked())
   {
      //// Query mode
      updateInterfaceQueryTagsList();
      bool queryTagsSpecified = (myDataPtr->myInterface.myQueryTagsList.count() > 0);

      if ( !queryTagsSpecified && (myDataPtr->myEmptyQueryAction == MyTMSUUI_IF_NS::RetrieveNone) )
      {
         myDataPtr->myCurrentFilesList.clear();
         setNavEnabledStates();
         beginDisplayList(true);
         return;
      }
      //// else

      myDataPtr->myInterface.retrieveFilesList(queryTagsSpecified);
   }
   else
   {
      if (myDataPtr->getCurrentFilename().isEmpty())
      {
         //// TODO-MAINT: Set status bar error string?
         qWarning("No current image file to apply tags");
         return;
      }
      //// else

      //// Build lists of tags to set/unset
      QStringList tagsToSet;
      QStringList tagsToUnset;
      for (MyTMSUUI_TagWidget* tagWidget : getTagWidgetList())
      {
         MyTMSUUI_Tagged_NS::CheckedState checkedState = tagWidget->getCheckedState();
         if (checkedState == MyTMSUUI_Tagged_NS::ToBeSetExplicitTag || checkedState == MyTMSUUI_Tagged_NS::ToBeSetBothTag)
         {
            QString tagToSetString = tagWidget->getTagName();
            if (tagWidget->usesValues())
            {
               if (tagWidget->getValue() == tagWidget->getValuePlaceholderText())
               {
                  qCritical("Cannot set tag %s=%s <---PLACEHOLDER TEXT", qUtf8Printable(tagToSetString), qUtf8Printable(tagWidget->getValue()));
                  continue;
               }
               else
               {
                  tagToSetString += "=";
                  tagToSetString += tagWidget->getValue();
               }
            }
            tagsToSet << tagToSetString;
         }
         else if (checkedState == MyTMSUUI_Tagged_NS::ToBeUnchecked || checkedState == MyTMSUUI_Tagged_NS::ToBeSetImpliedTag)
         {
            QString tagToUnsetString = tagWidget->getTagName();
            if (tagWidget->usesValues())
            {
               if (tagWidget->getValue() == tagWidget->getValuePlaceholderText())
               {
                  qCritical("Cannot unset tag %s=%s <---PLACEHOLDER TEXT", qUtf8Printable(tagToUnsetString), qUtf8Printable(tagWidget->getValue()));
                  continue;
               }
               else
               {
                  tagToUnsetString += "=";
                  tagToUnsetString += tagWidget->getValue();
               }
            }
            tagsToUnset << tagToUnsetString;
         }
      }

      myDataPtr->myInterface.myTagsToSetList = tagsToSet;
      myDataPtr->myInterface.myTagsToUnsetList = tagsToUnset;

      //// Begin tag set/unset sequence with setting tags
      //// (setTags() and unsetTags() will appropriately handle their empty lists)
      myDataPtr->myInterface.setTags(myDataPtr->getCurrentFilename());
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::applyTagWidgetToShortList(MyTMSUUI_TagWidget* tagWidgetPtr,
                                                    MyTMSUUI_MainWin_NS::ShortListModAction action)
{
   if (tagWidgetPtr == nullptr)
   {
      qCritical("Null tagWidget pointer");
      return;
   }
   //// else

   MyTMSUUI_TagWidget* cloneTagWidget = findCloneTagWidget(tagWidgetPtr);

   switch (action)
   {
    case MyTMSUUI_MainWin_NS::SL_MOD_ADD:
      if (cloneTagWidget == nullptr)
      {
         ////------------------------------------
         //// Add new clone widget to short list
         ////------------------------------------

         //// First ensure there's a layout
         bool setNewLayout = false;
         QVBoxLayout* tagWidgetsVLayout = (QVBoxLayout*)myGuiPtr->myShortTagsParentWidget->layout();
         if (tagWidgetsVLayout == nullptr)
         {
            tagWidgetsVLayout = new QVBoxLayout;
            tagWidgetsVLayout->setAlignment(Qt::AlignTop);
            setNewLayout = true;
         }

         //// Create new tagWidget
         MyTMSUUI_TagWidget* newTagWidget = new MyTMSUUI_TagWidget(myGuiPtr->myShortTagsParentWidget);
         newTagWidget->setObjectName(QString("shortListTagWidget") + QString::number(myShortListWidgetNum++));

         //// Copy tag data from main tagWidget
         MyTMSUUI_TagData* tagData = tagWidgetPtr->getTagData();
         newTagWidget->configure(tagData);
         delete tagData;

         //// Copy specific sub-widget states
         newTagWidget->setCheckedState(tagWidgetPtr->getCheckedState(), true);
         newTagWidget->setValue(tagWidgetPtr->getValue());

         //// Insert in sorted order
         bool inserted = false;
         QString newTagName = newTagWidget->getTagName();
         for (int idx = 0; idx < tagWidgetsVLayout->count(); ++idx)
         {
            if (tagWidgetsVLayout->itemAt(idx)->widget() == nullptr)
            {
               //// No widget; skip!
               continue;
            }
            //// else

            QWidget* wdgt = tagWidgetsVLayout->itemAt(idx)->widget();
            QString existingTagName = ((MyTMSUUI_TagWidget*)wdgt)->getTagName();
            if (newTagName.localeAwareCompare(existingTagName) <= 0)
            {
               tagWidgetsVLayout->insertWidget(idx, newTagWidget);
               inserted = true;
               break;
            }
         }

         if (!inserted)
         {
            tagWidgetsVLayout->addWidget(newTagWidget);
         }

         //// Establish connections
         connect(newTagWidget, SIGNAL(      shortTagClicked(const QString&, bool)),
                         this,   SLOT(handleShortTagClicked(const QString&, bool)) );
         connect(newTagWidget, SIGNAL(          valueIndexChanged(const QString&, int)),
                         this,   SLOT(handleShortTagValIdxChanged(const QString&, int)) );

         newTagWidget->resetUserClicked();

         if (setNewLayout)
         {
            myGuiPtr->myShortTagsParentWidget->setLayout(tagWidgetsVLayout);
         }
      }
      else
      {
         qWarning("Clone widget for tag %s already exists", qUtf8Printable(cloneTagWidget->getTagName()));
         //// TODO-MAINT: Update existing widget?
      }

      break;

    case MyTMSUUI_MainWin_NS::SL_MOD_REMOVE:
      if (cloneTagWidget == nullptr)
      {
         //qWarning("No clone widget for tag %s found; cannot 'remove'", qUtf8Printable(tagWidgetPtr->getTagName()));
         break;
      }
      //// else
      {
         ////-------------------------------------
         //// Remove clone widget from short list
         ////-------------------------------------
         QVBoxLayout* tagWidgetsVLayout = (QVBoxLayout*)myGuiPtr->myShortTagsParentWidget->layout();

         if (tagWidgetsVLayout == nullptr)
         {
            qCritical("No layout set for 'Short' Tags Parent Widget");
            return;
         }
         //// else

         for (int idx = 0; idx < tagWidgetsVLayout->count(); ++idx)
         {
            if (tagWidgetsVLayout->itemAt(idx)->widget() == nullptr)
            {
               //// No widget; skip!
               continue;
            }
            //// else

            QWidget* wdgt = tagWidgetsVLayout->itemAt(idx)->widget();
            if (((MyTMSUUI_TagWidget*)wdgt)->getTagName() == tagWidgetPtr->getTagName())
            {
               //// Found it!
               QLayoutItem* layoutItem = tagWidgetsVLayout->takeAt(idx);
               delete layoutItem->widget();
               delete layoutItem;
               break;
            }
         }
      }

      break;

    case MyTMSUUI_MainWin_NS::SL_MOD_UPDATE:
      {
         MyTMSUUI_Tagged_NS::CheckedState origTagChkdState = tagWidgetPtr->getCheckedState();
         if (cloneTagWidget == nullptr)
         {
            if (origTagChkdState != MyTMSUUI_Tagged_NS::Unchecked)
            {
               //// Add the missing clone
               applyTagWidgetToShortList(tagWidgetPtr, MyTMSUUI_MainWin_NS::SL_MOD_ADD);
            }
         }
         else if (origTagChkdState == MyTMSUUI_Tagged_NS::Unchecked) 
         {
            //// Remove clone from the list
            applyTagWidgetToShortList(tagWidgetPtr, MyTMSUUI_MainWin_NS::SL_MOD_REMOVE);
         }
         else
         {
            //// Update clone widget in short list; copy checked state
            cloneTagWidget->setCheckedState(origTagChkdState, true);
            cloneTagWidget->resetUserClicked();
         }
      }

      break;

    default:
      qCritical("Unhandled action: %d", action);
      return;
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::beginDisplayList(bool emptyListIsOK)
{
   ENSURE_DATA_PTR("cannot access files list")

   statusBar()->clearMessage();

   if (myDataPtr->myCurrentFilesList.isEmpty())
   {
      myGuiPtr->myImageWidget->clear();
      uncheckAllTagWidgets();
      if (emptyListIsOK)
      {
         myGuiStatusBarNormalLabel->setText("No images due to no tags in query");
         myGuiStatusBarErrorLabel->setText("");
      }
      else
      {
         myGuiStatusBarNormalLabel->setText("");
         myGuiStatusBarErrorLabel->setText("No image files in current base directory");
      }
      return;
   }
   //// else

   goToImage(1);

   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::buildImpliedTagChainsList(QList<MyTMSUUI_TaggedValue>* listToBuild,
                                                    const MyTMSUUI_TaggedValue& impliesTaggedValue)
{
   ENSURE_DATA_PTR("cannot access implications list")

   for(MyTMSUUI_TaggedValue impliedTaggedValue : myDataPtr->getImpliedTaggedValuesList(impliesTaggedValue))
   {
      if (!listToBuild->contains(impliedTaggedValue))
      {
         listToBuild->append(impliedTaggedValue);
         buildImpliedTagChainsList(listToBuild, impliedTaggedValue);
      }
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::clearTagWidgets()
{
   QVBoxLayout* tagWidgetsVLayout = (QVBoxLayout*)myGuiPtr->myTagsParentWidget->layout();

   if (tagWidgetsVLayout == nullptr)
   {
      qWarning("No layout set for Tags Parent Widget");
      return;
   }
   //// else

   int idx = tagWidgetsVLayout->count() - 1;

   while (idx >= 0)
   {
      if (tagWidgetsVLayout->itemAt(idx)->widget() != nullptr)
      {
         QLayoutItem* layoutItem = tagWidgetsVLayout->takeAt(idx);
         delete layoutItem->widget();
         delete layoutItem;
      }

      idx--;
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::closeEvent(QCloseEvent* event)
{
   // Call base class event handler
   QMainWindow::closeEvent(event);

   // Log closing
   qInfo("Closing Main Window\n");
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::doAbout()
{
   QMessageBox aboutDialog;

   //// TODO-FUTURE: Get text from resource (qrc)
   aboutDialog.setWindowTitle("About My TMSU UI");
   aboutDialog.setIcon(QMessageBox::Information);
   aboutDialog.setText("<h2>My TMSU UI</h2>");
   aboutDialog.setInformativeText("<strong>Version 0.1.0</strong><br />By Jess Odum");

   //// TODO-MAINT: Any details to provide?
   // aboutDialog.setDetailedText("This is the detailed text");

   aboutDialog.exec();
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::doOpenUserManual()
{
   //// TODO-FUTURE: Create an actual User Manual page. Get URL from resource (qrc).
   QDesktopServices::openUrl(QUrl("https://github.com/jessodum/my-tmsu-ui/blob/main/README.md"));
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::doSelectBaseDir()
{
   ENSURE_DATA_PTR("cannot set a base directory")

   //// Keep track of change to the selected base directory
   QString oldDir(myDataPtr->myCurrentBaseDir.absolutePath());

   //// Construct directory selection dialog
   QFileDialog dirSelector(this);
   dirSelector.setFileMode(QFileDialog::Directory);
   dirSelector.setOption(QFileDialog::ShowDirsOnly);
   dirSelector.setDirectory(myDataPtr->myCurrentBaseDir);

   //// Call (display) the dir selection dialog and get result
   QStringList selectedFiles;
   if (dirSelector.exec() == QDialog::Accepted)
   {
      selectedFiles = dirSelector.selectedFiles();
   }

   //// See if a (new) directory was selected
   if (selectedFiles.size() > 0)
   {
      QString newDir(selectedFiles[0]);
      if (newDir != oldDir)
      {
         //// Update the data object
         myDataPtr->myCurrentBaseDir.setPath(newDir);

         //// Notify other elements of the app that the selected dir changed
         emit dataBaseDirChanged(newDir);
      }
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::doUpdateRecurse(bool newRecurseState)
{
   ENSURE_DATA_PTR("cannot set the 'recurse enabled' flag")

   myDataPtr->myRecurseEnabled = newRecurseState;
   setStatusUpdating();
   updateInterfaceFilesList();
   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
MyTMSUUI_TagWidget* MyTMSUUI_MainWindow::findCloneTagWidget(MyTMSUUI_TagWidget* origTagWidget)
{
   MyTMSUUI_TagWidget* retval = nullptr;
   if (origTagWidget == nullptr)
   {
      qCritical("Null original tag widget");
      return retval;
   }
   //// else

   //// Specify 'true' to get from the "Short" list
   TagWidgetList tagWidgetsList = getTagWidgetList(true);

   for (MyTMSUUI_TagWidget* tagWidget : tagWidgetsList)
   {
      if (tagWidget->getTagName() == origTagWidget->getTagName())
      {
         //// Found it!
         retval = tagWidget;
         break;
      }
   }

   return retval;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
MyTMSUUI_TagWidget* MyTMSUUI_MainWindow::findTagWidget(const QString& tagName)
{
   MyTMSUUI_TagWidget* retval = nullptr;
   TagWidgetList tagWidgetsList = getTagWidgetList();

   for (MyTMSUUI_TagWidget* tagWidget : tagWidgetsList)
   {
      if (tagWidget->getTagName() == tagName)
      {
         //// Found it!
         retval = tagWidget;
         break;
      }
   }

   return retval;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::firstButtonClicked()
{
   goToImage(1);
   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
TagWidgetList MyTMSUUI_MainWindow::getTagWidgetList(bool useShortList)
{
   TagWidgetList retList;
   QVBoxLayout* tagWidgetsVLayout = useShortList ? (QVBoxLayout*)myGuiPtr->myShortTagsParentWidget->layout()
                                                 : (QVBoxLayout*)myGuiPtr->myTagsParentWidget->layout();

   if (tagWidgetsVLayout == nullptr)
   {
      // QString qualifier = useShortList ? " Short" : "";
      // qWarning("No layout set for%s Tags Parent Widget", qUtf8Printable(qualifier) );
      return retList; //// Empty list
   }
   //// else

   for (int idx = 0; idx < tagWidgetsVLayout->count(); ++idx)
   {
      QLayoutItem* layoutItem = tagWidgetsVLayout->itemAt(idx);
      QWidget* widget = layoutItem->widget();

      if (widget == nullptr)
      {
         continue;
      }
      //// else

      retList << (MyTMSUUI_TagWidget*)widget;
   }

   return retList;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::goToImage(qsizetype number)
{
   ENSURE_DATA_PTR("cannot access files list")

   if (myDataPtr->myCurrentFilesList.isEmpty())
   {
      statusBar()->showMessage("INTERNAL ERROR: Files list is empty - cannot go to image");
      return;
   }
   //// else

   if (number > myDataPtr->myCurrentFilesList.size())
   {
      goToImage(1);
      return;
   }
   //// else

   if (number < 1)
   {
      goToLastImage();
      return;
   }
   //// else

   myDataPtr->myCurrentImageNum = number;
   updateUiForCurrentImage();

   statusBar()->clearMessage();
   QString currentFilename = myDataPtr->getCurrentFilename();
   if (currentFilename.isEmpty())
   {
      qCritical("Failed to get current filename (how did this happen?)");
      currentFilename = "(GET FILENAME FAILED)";
   }

   QString imgNumberStatus("Image ");
   imgNumberStatus += QString::number(number);
   imgNumberStatus += " of ";
   imgNumberStatus += QString::number(myDataPtr->myCurrentFilesList.size());
   imgNumberStatus += ": ";
   imgNumberStatus += currentFilename;
   myGuiStatusBarNormalLabel->setText(imgNumberStatus);
   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::goToLastImage()
{
   ENSURE_DATA_PTR("cannot access files list")

   if (myDataPtr->myCurrentFilesList.isEmpty())
   {
      statusBar()->showMessage("INTERNAL ERROR: Files list is empty - cannot go to image");
      return;
   }
   //// else

   goToImage(myDataPtr->myCurrentFilesList.size());
   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::handleMainTagToggled(const QString& tagName, bool byUserClick)
{
   bool userClickedForQuery = false;
   if (!myToggleOtherTagsAllowed)
   {
      if (byUserClick && myGuiPtr->myQueryRadioButton->isChecked())
      {
         userClickedForQuery = true;
      }
      else
      {
         //// TODO-MAINT: Is this condition an issue?
         return;
      }
   }

   if (!byUserClick)
   {
      //// TODO-MAINT: Is this condition an issue?
      return;
   }
   //// else

   MyTMSUUI_TagWidget* tagWidget = findTagWidget(tagName);

   if (tagWidget == nullptr)
   {
      qCritical("Cannot handle tag toggled for %s; could not find widget for that tag", qUtf8Printable(tagName));
      return;
   }
   //// else

   if (userClickedForQuery)
   {
      tagWidget->setCheckedState( tagWidget->isChecked() ? MyTMSUUI_Tagged_NS::SetExplicitTag
                                                         : MyTMSUUI_Tagged_NS::Unchecked );

      if ( !tagWidget->isChecked() && tagWidget->usesValues() )
      {
         tagWidget->resetValuesSelect();
      }

      applyTagWidgetToShortList(tagWidget, tagWidget->isChecked() ? MyTMSUUI_MainWin_NS::SL_MOD_ADD
                                                                  : MyTMSUUI_MainWin_NS::SL_MOD_REMOVE);

      return;
   }
   //// else

   MyTMSUUI_TaggedValue taggedValueInput;
   taggedValueInput.myTagName = tagName;
   taggedValueInput.myValue = tagWidget->getValue();

   switch (tagWidget->getCheckedState()) //// The "old" Checked State
   {
      case MyTMSUUI_Tagged_NS::Unchecked : //// GRAY --> GREEN
         tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeSetExplicitTag);
         break;

      case MyTMSUUI_Tagged_NS::ToBeUnchecked : //// RED --> BLUE
         tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::SetExplicitTag);
         break;

      case MyTMSUUI_Tagged_NS::SetExplicitTag : //// BLUE --> RED
         tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeUnchecked);
         break;

      case MyTMSUUI_Tagged_NS::ToBeSetExplicitTag : //// GREEN --> GRAY
         tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::Unchecked);
         break;

      case MyTMSUUI_Tagged_NS::SetImpliedTag : //// PURPLE --> YELLOW
         tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeSetBothTag, true);
         break;

      case MyTMSUUI_Tagged_NS::ToBeSetImpliedTag : //// PINK --> YELLOW
         tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeSetBothTag, true);
         break;

      case MyTMSUUI_Tagged_NS::SetBothTag : //// ORANGE --> PINK
         tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeSetImpliedTag, true);
         break;

      case MyTMSUUI_Tagged_NS::ToBeSetBothTag : //// YELLOW --> PINK
         tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeSetImpliedTag, true);
         break;

      default:
         qCritical("Unhandled old checked state %d", tagWidget->getCheckedState());
         return;
   }

   //// Update "short" list tag widget(s)
   applyTagWidgetToShortList(tagWidget, MyTMSUUI_MainWin_NS::SL_MOD_UPDATE);

   //// Update "implies" chain(s)
   QList<MyTMSUUI_TaggedValue> impliedTaggedValuesToUpdateList;
   buildImpliedTagChainsList(&impliedTaggedValuesToUpdateList, taggedValueInput);

   for (MyTMSUUI_TaggedValue impliedTaggedValue : impliedTaggedValuesToUpdateList)
   {
      MyTMSUUI_TagWidget* tWidget = findTagWidget(impliedTaggedValue.myTagName);
      if (tWidget == nullptr)
      {
         qCritical("Could not find a tag widget for implied tag name %s", qUtf8Printable(impliedTaggedValue.myTagName));
         continue;
      }
      //// else

      switch (tagWidget->getCheckedState()) //// The "new" Checked State
      {
         case MyTMSUUI_Tagged_NS::ToBeSetExplicitTag : //// GRAY --> GREEN
            switch (tWidget->getCheckedState())
            {
             case MyTMSUUI_Tagged_NS::Unchecked : //// Implies Tag: GRAY --> PINK
              tWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeSetImpliedTag, true);
              if ( ! impliedTaggedValue.myValue.isEmpty() )
              {
                 tWidget->setValue(impliedTaggedValue.myValue);
              }
              break;
             case MyTMSUUI_Tagged_NS::SetExplicitTag :      //// Implies Tag: BLUE --> YELLOW
             case MyTMSUUI_Tagged_NS::ToBeSetExplicitTag : //// Implies Tag: GREEN --> YELLOW
              tWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeSetBothTag, true);
              break;
             default:
              //// TODO-MAINT: Is this condition an issue?
              break;
            }
            break;

         case MyTMSUUI_Tagged_NS::SetExplicitTag : //// RED --> BLUE
            switch (tWidget->getCheckedState())
            {
             case MyTMSUUI_Tagged_NS::ToBeUnchecked : //// Implies Tag: RED --> PURPLE
              tWidget->setCheckedState(MyTMSUUI_Tagged_NS::SetImpliedTag, true);
              if ( ! impliedTaggedValue.myValue.isEmpty() )
              {
                 tWidget->setValue(impliedTaggedValue.myValue);
              }
              break;
             case MyTMSUUI_Tagged_NS::ToBeSetImpliedTag : //// Implies Tag: PINK --> ORANGE
              tWidget->setCheckedState(MyTMSUUI_Tagged_NS::SetBothTag, true);
              break;
             default:
              //// TODO-MAINT: Is this condition an issue?
              break;
            }
            break;

         case MyTMSUUI_Tagged_NS::ToBeUnchecked : //// BLUE --> RED
            switch (tWidget->getCheckedState())
            {
             case MyTMSUUI_Tagged_NS::SetImpliedTag : //// Implies Tag: PURPLE --> RED 
              tWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeUnchecked, true);
              break;
             case MyTMSUUI_Tagged_NS::SetBothTag : //// Implies Tag: ORANGE --> PINK
              tWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeSetImpliedTag, true);
              break;
             default:
              //// TODO-MAINT: Is this condition an issue?
              break;
            }
            break;

         case MyTMSUUI_Tagged_NS::Unchecked : //// GREEN --> GRAY
            switch (tWidget->getCheckedState())
            {
             case MyTMSUUI_Tagged_NS::ToBeSetImpliedTag : //// Implies Tag: PINK --> GRAY 
              tWidget->setCheckedState(MyTMSUUI_Tagged_NS::Unchecked, true);
              break;
             case MyTMSUUI_Tagged_NS::ToBeSetBothTag : //// Implies Tag: YELLOW --> GREEN
              tWidget->setCheckedState(MyTMSUUI_Tagged_NS::ToBeSetExplicitTag, true);
              break;
             default:
              //// TODO-MAINT: Is this condition an issue?
              break;
            }
            break;

         case MyTMSUUI_Tagged_NS::ToBeSetBothTag : //// PURPLE/PINK --> YELLOW
         case MyTMSUUI_Tagged_NS::ToBeSetImpliedTag : //// ORANGE/YELLOW --> PINK
            //// TODO-MAINT: Is this condition an issue?
            break;

         default:
            qCritical("Unhandled new checked state %d", tagWidget->getCheckedState());
            break;

      } //// End switch(tagWidget->getCheckedState())

      //// Update "short" list tag widget(s)
      applyTagWidgetToShortList(tWidget, MyTMSUUI_MainWin_NS::SL_MOD_UPDATE);

   } //// End foreach impliedTagName
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::handleMainTagValIdxChanged(const QString& tagName, int index)
{
   MyTMSUUI_TagWidget* mainTagWidget = findTagWidget(tagName);
   if (mainTagWidget == nullptr)
   {
      qCritical("Cannot find original widget for %s", qUtf8Printable(tagName));
      return;
   }
   //// else

   //// Does a clone tag widget exist?
   MyTMSUUI_TagWidget* cloneTagWidget = findCloneTagWidget(mainTagWidget);

   if (cloneTagWidget == nullptr)
   {
      //// No clone tag widget found; just ignore.
      return;
   }
   //// else

   if (index < 0)
   {
      cloneTagWidget->resetValuesSelect();
      return;
   }
   //// else

   //// Use the value (string) from the original widget
   if ( !(mainTagWidget->usesValues()) )
   {
      qCritical("Index changed for original widget that doesn't use values? (Tag: %s)", qUtf8Printable(tagName));
      return;
   }
   //// else

   //// Copy new value string
   cloneTagWidget->setValue(mainTagWidget->getValue());
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::handleShortTagClicked(const QString& tagName, bool byUserClick)
{
   if (!byUserClick)
   {
      //// TODO-MAINT: Is this condition an issue?
      return;
   }
   //// else

   //// Treat as though main tag widget clicked
   MyTMSUUI_TagWidget* mainTagWidget = findTagWidget(tagName);

   if (mainTagWidget == nullptr)
   {
      qCritical("Cannot handle short list tag clicked for %s; could not find main widget for that tag", qUtf8Printable(tagName));
      return;
   }
   //// else

   mainTagWidget->clickCheckbox();
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::handleShortTagValIdxChanged(const QString& tagName, int index)
{
   //// Find original tag widget
   MyTMSUUI_TagWidget* mainTagWidget = findTagWidget(tagName);

   if (mainTagWidget == nullptr)
   {
      qCritical("Cannot find original widget for %s", qUtf8Printable(tagName));
      return;
   }
   //// else

   if (index < 0)
   {
      mainTagWidget->resetValuesSelect();
      return;
   }
   //// else

   //// Use the value (string) from the clone widget
   MyTMSUUI_TagWidget* cloneTagWidget = findCloneTagWidget(mainTagWidget);
   if (cloneTagWidget == nullptr)
   {
      qCritical("Unable to determine value string; cannot find clone widget for %s", qUtf8Printable(tagName));
      return;
   }
   //// else

   if ( !(cloneTagWidget->usesValues()) )
   {
      qCritical("Index changed for clone widget that doesn't use values? (Tag: %s)", qUtf8Printable(tagName));
      return;
   }
   //// else

   //// Copy new value string
   mainTagWidget->setValue(cloneTagWidget->getValue());
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::interfaceGoneIdle(MyTMSUUI_IF_NS::ProcState lastState, bool withError)
{
   statusBar()->clearMessage();

   if (withError)
   {
      myGuiStatusBarErrorLabel->setText(myDataPtr->myInterface.getError());
   }
   else
   {
      myGuiStatusBarErrorLabel->setText("");
   }

   switch (lastState)
   {
      case MyTMSUUI_IF_NS::InfoQuery:
         if (withError)
         {
            clearTagWidgets();
            myDataPtr->myCurrentFilesList.clear();
            setNavEnabledStates();
            myGuiPtr->myImageWidget->clear();
            myGuiStatusBarNormalLabel->setText("");
            myGuiPtr->myApplyButton->setEnabled(false);
         }
         else
         {
            //// Update list of image files based on new directory
            setStatusUpdating();
            updateInterfaceFilesList();
         }

         break;

      case MyTMSUUI_IF_NS::ImpliesDBQuery:
         if (withError)
            break;

         //// (Re-)build tag widgets
         setStatusUpdating();
         rebuildTagWidgets();

         //// Update list of image files based on new directory/database
         myGuiPtr->myApplyButton->setEnabled(true);
         updateInterfaceFilesList();
         break;

      case MyTMSUUI_IF_NS::BuildFilesList:
         if (withError)
         {
            myDataPtr->myCurrentFilesList.clear();
            setNavEnabledStates();
            uncheckAllTagWidgets();
            myGuiPtr->myImageWidget->clear();
            myGuiStatusBarNormalLabel->setText("");
         }
         else
         {
            //// Update widgets for new files list
            setStatusUpdating();
            prepFilesListForDisplay();
            beginDisplayList();
         }

         break;

      case MyTMSUUI_IF_NS::RetrieveFileTags:
         if (withError)
            //// TODO-MAINT: handle RetrieveFileTags withError?
            break;

         setTaggedValuesInWidgets();
         break;

      case MyTMSUUI_IF_NS::SetTags:
      case MyTMSUUI_IF_NS::UnsetTags:
         if (withError)
         {
            //// TODO-MAINT: handle RetrieveFileTags withError?
         }
         else
         {
            statusBar()->showMessage("Applied", 4000); //// 4 seconds
         }

         myDataPtr->myInterface.retrieveFileTags(myDataPtr->getCurrentFilename());
         break;

      default:
         break;
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
bool MyTMSUUI_MainWindow::isCurrentImageAnim()
{
   ENSURE_DATA_PTR_RETURN_FALSE("cannot access image data")

   QString imgFullPath = myDataPtr->getCurrentFileFullPath();
   if (imgFullPath.isEmpty())
   {
      qCritical("Failed to get current filename (how did this happen?)");
      return false;
   }
   //// else

   QImageReader imgReader(imgFullPath);
   imgReader.setDecideFormatFromContent(true);
   return ( imgReader.supportsAnimation() && (imgReader.imageCount() > 1) );
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::lastButtonClicked()
{
   ENSURE_DATA_PTR("cannot access files list")

   goToImage(myDataPtr->myCurrentFilesList.size());
   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::nextButtonClicked()
{
   ENSURE_DATA_PTR("cannot access current image number")

   goToImage(myDataPtr->myCurrentImageNum + 1);
   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::prepFilesListForDisplay()
{
   ENSURE_DATA_PTR("cannot access files list")

   //// Use a copy of the File List for iterating, as we'll be removing unwanted
   //// (i.e., non-image) files from the Data list.
   QStringList tmpFilesList = myDataPtr->myCurrentFilesList;

   for (QString currentFile : tmpFilesList)
   {
      QString currFileFullPath = myDataPtr->myCurrentBaseDir.absolutePath() + "/" + currentFile;
      QImageReader imgReader(currFileFullPath);
      imgReader.setDecideFormatFromContent(true);

      QByteArray imgFormat = imgReader.format();
      if (imgFormat.isEmpty())
      {
         qWarning("(%s) %s; removing it from the files list", qUtf8Printable(imgReader.errorString()), qUtf8Printable(currentFile));
         if (!(myDataPtr->myCurrentFilesList.removeOne(currentFile)) )
         {
            qWarning("Failed to remove it from the files list!");
         }
      }
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::prevButtonClicked()
{
   ENSURE_DATA_PTR("cannot access current image number")

   goToImage(myDataPtr->myCurrentImageNum - 1);
   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioAllClicked()
{
   ENSURE_DATA_PTR("cannot set the 'empty query action' flag")
   myDataPtr->myEmptyQueryAction = MyTMSUUI_IF_NS::RetrieveAll;
   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioNoneClicked()
{
   ENSURE_DATA_PTR("cannot set the 'empty query action' flag")
   myDataPtr->myEmptyQueryAction = MyTMSUUI_IF_NS::RetrieveNone;
   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioQueryClicked()
{
   myGuiPtr->myRetrieveOptionsGroupBox->setEnabled(true);
   myGuiPtr->myApplyButton->setText("Get");

   //// Prevent handle*TagToggled from updating other tags' checkmarks during Query mode.
   myToggleOtherTagsAllowed = false;

   uncheckAllTagWidgets();

   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioSetTagsClicked()
{
   myGuiPtr->myRetrieveOptionsGroupBox->setEnabled(false);
   myGuiPtr->myApplyButton->setText("Apply");
   myToggleOtherTagsAllowed = true; //// Restore

   ENSURE_DATA_PTR("cannot access files list")

   if (!myDataPtr->myCurrentFilesList.isEmpty())
   {
      emit imageUpdated(myDataPtr->getCurrentFilename());
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioUntaggedClicked()
{
   ENSURE_DATA_PTR("cannot set the 'empty query action' flag")
   myDataPtr->myEmptyQueryAction = MyTMSUUI_IF_NS::RetrieveUntagged;
   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::rebuildTagWidgets()
{
   //// Clear out any old widgets
   clearTagWidgets();

   size_t widgetNum = 0;
   bool setNewLayout = false;

   MyTMSUUI_TagWidget* newTagWidget;
   QVBoxLayout* tagWidgetsVLayout = (QVBoxLayout*)myGuiPtr->myTagsParentWidget->layout();
   if (tagWidgetsVLayout == nullptr)
   {
      tagWidgetsVLayout = new QVBoxLayout;
      setNewLayout = true;
   }

   //// Iterate over list of tags from Data Ptr
   for (MyTMSUUI_TagData* tagData : myDataPtr->myTagsList)
   {
      newTagWidget = new MyTMSUUI_TagWidget(myGuiPtr->myTagsParentWidget);
      newTagWidget->setObjectName(QString("tagWidget") + QString::number(widgetNum++));

      newTagWidget->configure(tagData);

      tagWidgetsVLayout->addWidget(newTagWidget);

      connect(newTagWidget, SIGNAL(          tagToggled(const QString&, bool)),
                      this,   SLOT(handleMainTagToggled(const QString&, bool)) );
      connect(newTagWidget, SIGNAL(      tagToggled(const QString&, bool)),
              newTagWidget,   SLOT(resetUserClicked()) );
      connect(newTagWidget, SIGNAL(         valueIndexChanged(const QString&, int)),
                      this,   SLOT(handleMainTagValIdxChanged(const QString&, int)) );
   }

   if (setNewLayout)
   {
      myGuiPtr->myTagsParentWidget->setLayout(tagWidgetsVLayout);
   }
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::resetButtonClicked()
{
   if (myGuiPtr->myQueryRadioButton->isChecked())
   {
      uncheckAllTagWidgets();
   }
   else
   {
      setTaggedValuesInWidgets();
   }
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::scrollToBottomClicked()
{
   QWidget* innerWidget = myGuiPtr->myTagsScrollArea->widget();

   int bottom = innerWidget->height();
   myGuiPtr->myTagsScrollArea->ensureVisible(0, bottom);
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::scrollToTopClicked()
{
   myGuiPtr->myTagsScrollArea->ensureVisible(0, 0);
}

//// --------------------------------------------------------------------------
//// (public)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::setDataObj(MyTMSUUI_Data* dataPtr)
{
   myDataPtr = dataPtr;
   if (myDataPtr != nullptr)
   {
      //// this: Base dir changed
      connect(                     this, SIGNAL(dataBaseDirChanged(const QString&)),
              &(myDataPtr->myInterface),   SLOT(      doNewBaseDir(const QString&)) );

      //// this: Image updated
      connect(                     this, SIGNAL(    imageUpdated(const QString&)),
              &(myDataPtr->myInterface),   SLOT(retrieveFileTags(const QString&)) );

      //// I/f: Gone idle
      connect(&(myDataPtr->myInterface), SIGNAL(         goneIdle(MyTMSUUI_IF_NS::ProcState, bool)),
                                   this,   SLOT(interfaceGoneIdle(MyTMSUUI_IF_NS::ProcState, bool)) );

      emit dataBaseDirChanged(myDataPtr->myCurrentBaseDir.absolutePath());
   }
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::setNavEnabledStates()
{
   ENSURE_DATA_PTR("cannot access current image number")

   if (myDataPtr->myCurrentFilesList.size() < 1)
   {
      myGuiPtr->myFirstImgButton->setEnabled(false);
      myGuiPtr->myPrevImgButton->setEnabled(false);
      myGuiPtr->myNextImgButton->setEnabled(false);
      myGuiPtr->myLastImgButton->setEnabled(false);
      return;
   }
   //// else

   myGuiPtr->myFirstImgButton->setEnabled(myDataPtr->myCurrentImageNum > 1);
   myGuiPtr->myPrevImgButton->setEnabled(myDataPtr->myCurrentFilesList.size() > 1);
   myGuiPtr->myNextImgButton->setEnabled(myDataPtr->myCurrentFilesList.size() > 1);
   myGuiPtr->myLastImgButton->setEnabled(myDataPtr->myCurrentImageNum < myDataPtr->myCurrentFilesList.size());
   return;
}

//// --------------------------------------------------------------------------
//// (protected SLOT)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::setStatusUpdating()
{
   statusBar()->showMessage("Updating...");
   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::setTaggedValuesInWidgets()
{
   ENSURE_DATA_PTR("cannot access interface object")

   if (myGuiPtr->myQueryRadioButton->isChecked())
   {
      qWarning("Attempt to set tagged values while 'Query' radio button set is invalid");
      return;
   }

   //// Prevent handle*TagToggled from updating other tags' checkmarks during this procedure.
   myToggleOtherTagsAllowed = false;

   TagWidgetList tagWidgetsList = getTagWidgetList();

   //// First uncheck all tag widgets
   for (MyTMSUUI_TagWidget* tagWidget : tagWidgetsList)
   {
      //// Remove widgets from "short" list
      applyTagWidgetToShortList(tagWidget, MyTMSUUI_MainWin_NS::SL_MOD_REMOVE);
      tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::Unchecked, true);
      if (tagWidget->usesValues())
      {
         tagWidget->resetValuesSelect();
      }
   }

   for (MyTMSUUI_TaggedValue taggedVal : myDataPtr->myCurrentImgTaggedValList)
   {
      for (MyTMSUUI_TagWidget* tagWidget : tagWidgetsList)
      {
         if (tagWidget->getTagName() == taggedVal.myTagName)
         {
            tagWidget->setCheckedState(taggedVal.myCheckedState, true);
            if ( tagWidget->usesValues() && !taggedVal.myValue.isEmpty() )
            {
               tagWidget->setValue(taggedVal.myValue);
            }

            //// Add clone widget to "short" list
            applyTagWidgetToShortList(tagWidget, MyTMSUUI_MainWin_NS::SL_MOD_ADD);

            //// Found it; stop searching
            break;
         }
      }
   }

   myToggleOtherTagsAllowed = true; //// Restore

   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::uncheckAllTagWidgets()
{
   TagWidgetList tagWidgetsList = getTagWidgetList();
   for (MyTMSUUI_TagWidget* tagWidget : tagWidgetsList)
   {
      applyTagWidgetToShortList(tagWidget, MyTMSUUI_MainWin_NS::SL_MOD_REMOVE);
      tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::Unchecked, true);
      if (tagWidget->usesValues())
      {
         tagWidget->resetValuesSelect();
      }
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::updateInterfaceFilesList()
{
   ENSURE_DATA_PTR("cannot access interface object")

   bool queryTagsSpecified = false;

   if (myGuiPtr->myQueryRadioButton->isChecked())
   {
      updateInterfaceQueryTagsList();
      queryTagsSpecified = (myDataPtr->myInterface.myQueryTagsList.count() > 0);

      if ( !queryTagsSpecified && (myDataPtr->myEmptyQueryAction == MyTMSUUI_IF_NS::RetrieveNone) )
      {
         myDataPtr->myCurrentFilesList.clear();
         setNavEnabledStates();
         beginDisplayList(true);
         return;
      }

   }

   myDataPtr->myInterface.retrieveFilesList(queryTagsSpecified);
   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::updateInterfaceQueryTagsList()
{
   ENSURE_DATA_PTR("cannot access interface object")

   myDataPtr->myInterface.myQueryTagsList.clear();

   TagWidgetList twList = getTagWidgetList();

   for (MyTMSUUI_TagWidget* tagWidget : twList)
   {
      if (tagWidget->isChecked())
      {
         MyTMSUUI_TaggedValue newQueryTag;
         newQueryTag.myTagName = tagWidget->getTagName();
         newQueryTag.myValue = tagWidget->getValue();
         myDataPtr->myInterface.myQueryTagsList << newQueryTag;
      }
   }

   return;
}

//// --------------------------------------------------------------------------
//// (protected)
//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::updateUiForCurrentImage()
{
   ENSURE_DATA_PTR("cannot access image data")

   setNavEnabledStates();

   //// Clear previous content
   QMovie* oldMoviePtr = myGuiPtr->myImageWidget->movie();
   if (oldMoviePtr != nullptr)
   {
      delete oldMoviePtr;
   }

   myGuiPtr->myImageWidget->clear();

   //// Determine if image is animated.
   if (isCurrentImageAnim())
   {
      bool needsToShrink = false;
      QSize scaledSize;
      {
         QImageReader animImgReader(myDataPtr->getCurrentFileFullPath());
         //// TODO-FUTURE: Use a config system (qgetenv + QSettings + QCommandLineArgs) for max image height
         if (animImgReader.size().height() > MyTMSUUI_MainWin_NS::MAX_IMAGE_HEIGHT)
         {
            needsToShrink = true;
            QImage animFrame = animImgReader.read();
            scaledSize = (animFrame.scaledToHeight(MyTMSUUI_MainWin_NS::MAX_IMAGE_HEIGHT)).size();
         }
         //// TODO-MAINT: max image width?
      }

      QMovie* anim = new QMovie(myDataPtr->getCurrentFileFullPath());

      if (needsToShrink)
      {
         anim->setScaledSize(scaledSize);
      }

      myGuiPtr->myImageWidget->setMovie(anim);
      anim->start();
   }
   else
   {
      QPixmap img(myDataPtr->getCurrentFileFullPath());

      //// TODO-FUTURE: Use a config system (qgetenv + QSettings + QCommandLineArgs) for max image height
      if (img.height() > MyTMSUUI_MainWin_NS::MAX_IMAGE_HEIGHT)
      {
         QPixmap scaledImg = img.scaledToHeight(MyTMSUUI_MainWin_NS::MAX_IMAGE_HEIGHT);
         myGuiPtr->myImageWidget->setPixmap(scaledImg);
      }
      //// TODO-MAINT: max image width?
      else
      {
         myGuiPtr->myImageWidget->setPixmap(img);
      }
   }

   //// Read image's tags and set in tag widgets (if "Set" radio selected)
   if (myGuiPtr->mySetTagsRadioButton->isChecked())
   {
      emit imageUpdated(myDataPtr->getCurrentFilename());
   }

   return;
}

