#include "mytmsuui_mainwindow.h"
#include "ui_mytmsuui_mainwindow.h"
#include "mytmsuui_tagwidget.h"
#include <QFileDialog>
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

//// ==========================================================================
//// Public
//// ==========================================================================

//// --------------------------------------------------------------------------
MyTMSUUI_MainWindow::MyTMSUUI_MainWindow(QWidget* parent)
 : QMainWindow(parent)
 , myGuiPtr(new Ui::MyTMSUUI_MainWindow)
 , myGuiStatusBarNormalLabel(nullptr)
 , myGuiStatusBarErrorLabel(nullptr)
 , myDataPtr(nullptr)
 , myToggleOtherTagsAllowed(true)
{
   //// TODO-FUTURE: Add feature to hide unchecked TagWidgets.

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
                                   this,   SLOT(doOpenHelpManual()) );

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
MyTMSUUI_MainWindow::~MyTMSUUI_MainWindow()
{
   delete myGuiPtr;
}

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

//// ==========================================================================
//// Protected
//// ==========================================================================

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::closeEvent(QCloseEvent* event)
{
   // Call base class event handler
   QMainWindow::closeEvent(event);

   // Log closing
   qInfo("Closing Main Window\n");
}

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

      connect(newTagWidget, SIGNAL(      tagToggled(const QString&, bool)),
                      this,   SLOT(handleTagToggled(const QString&, bool)) );
      connect(newTagWidget, SIGNAL(      tagToggled(const QString&, bool)),
              newTagWidget,   SLOT(resetUserClicked()) );
   }

   if (setNewLayout)
   {
      myGuiPtr->myTagsParentWidget->setLayout(tagWidgetsVLayout);
   }
}

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
void MyTMSUUI_MainWindow::setTaggedValuesInWidgets()
{
   ENSURE_DATA_PTR("cannot access interface object")

   if (myGuiPtr->myQueryRadioButton->isChecked())
   {
      qWarning("Attempt to set tagged values while 'Query' radio button set is invalid");
      return;
   }

   //// Prevent handleTagToggled from updating other tags' checkmarks during this procedure.
   myToggleOtherTagsAllowed = false;

   TagWidgetList tagWidgetsList = getTagWidgetList();

   //// First uncheck all tag widgets
   for (MyTMSUUI_TagWidget* tagWidget : tagWidgetsList)
   {
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

            //// Found it; stop searching
            break;
         }
      }
   }

   myToggleOtherTagsAllowed = true; //// Restore

   return;
}

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
void MyTMSUUI_MainWindow::uncheckAllTagWidgets()
{
   TagWidgetList tagWidgetsList = getTagWidgetList();
   for (MyTMSUUI_TagWidget* tagWidget : tagWidgetsList)
   {
      tagWidget->setCheckedState(MyTMSUUI_Tagged_NS::Unchecked, true);
      if (tagWidget->usesValues())
      {
         tagWidget->resetValuesSelect();
      }
   }

   return;
}

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
TagWidgetList MyTMSUUI_MainWindow::getTagWidgetList()
{
   TagWidgetList retList;
   QVBoxLayout* tagWidgetsVLayout = (QVBoxLayout*)myGuiPtr->myTagsParentWidget->layout();

   if (tagWidgetsVLayout == nullptr)
   {
      qWarning("No layout set for Tags Parent Widget");
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

//// ==========================================================================
//// Protected Slots
//// ==========================================================================

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
void MyTMSUUI_MainWindow::doOpenHelpManual()
{
   //// TODO-FUTURE: Open Help Manual
   qDebug("TODO doOpenHelpManual");
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::doAbout()
{
   //// TODO: About Dialog
   qDebug("TODO doAbout");
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::firstButtonClicked()
{
   goToImage(1);
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::prevButtonClicked()
{
   ENSURE_DATA_PTR("cannot access current image number")

   goToImage(myDataPtr->myCurrentImageNum - 1);
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::nextButtonClicked()
{
   ENSURE_DATA_PTR("cannot access current image number")

   goToImage(myDataPtr->myCurrentImageNum + 1);
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::lastButtonClicked()
{
   ENSURE_DATA_PTR("cannot access files list")

   goToImage(myDataPtr->myCurrentFilesList.size());
   return;
}

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
void MyTMSUUI_MainWindow::resetButtonClicked()
{
   //// TODO: resetButtonClicked
   qDebug("TODO resetButtonClicked");
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::scrollToBottomClicked()
{
   QWidget* innerWidget = myGuiPtr->myTagsScrollArea->widget();

   int bottom = innerWidget->height();
   myGuiPtr->myTagsScrollArea->ensureVisible(0, bottom);
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::scrollToTopClicked()
{
   myGuiPtr->myTagsScrollArea->ensureVisible(0, 0);
}

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
void MyTMSUUI_MainWindow::radioQueryClicked()
{
   myGuiPtr->myRetrieveOptionsGroupBox->setEnabled(true);
   myGuiPtr->myApplyButton->setText("Get");

   //// Prevent handleTagToggled from updating other tags' checkmarks during Query mode.
   myToggleOtherTagsAllowed = false;

   uncheckAllTagWidgets();

   return;
}

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
void MyTMSUUI_MainWindow::radioUntaggedClicked()
{
   ENSURE_DATA_PTR("cannot set the 'empty query action' flag")
   myDataPtr->myEmptyQueryAction = MyTMSUUI_IF_NS::RetrieveUntagged;
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioAllClicked()
{
   ENSURE_DATA_PTR("cannot set the 'empty query action' flag")
   myDataPtr->myEmptyQueryAction = MyTMSUUI_IF_NS::RetrieveAll;
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioNoneClicked()
{
   ENSURE_DATA_PTR("cannot set the 'empty query action' flag")
   myDataPtr->myEmptyQueryAction = MyTMSUUI_IF_NS::RetrieveNone;
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::setStatusUpdating()
{
   statusBar()->showMessage("Updating...");
   return;
}

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
void MyTMSUUI_MainWindow::handleTagToggled(const QString& tagName, bool byUserClick)
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

   //// Update "implies" chain(s)
   QList<MyTMSUUI_TaggedValue> impliedTaggedValuesToUpdateList;
   buildImpliedTagChainsList(&impliedTaggedValuesToUpdateList, taggedValueInput);

   for (MyTMSUUI_TaggedValue impliedTaggedValue : impliedTaggedValuesToUpdateList)
   {
      MyTMSUUI_TagWidget* tWidget = findTagWidget(impliedTaggedValue.myTagName);
      if (tWidget != nullptr)
      {
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
      } //// End if (tWidget != nullptr)
   } //// End foreach impliedTagName
}

