#include "mytmsuui_mainwindow.h"
#include "ui_mytmsuui_mainwindow.h"
#include "mytmsuui_tagwidget.h"
#include <QFileDialog>
#include <QImageReader>
#include <QtLogging>

//// --------------------------------------------------------------------------
MyTMSUUI_MainWindow::MyTMSUUI_MainWindow(QWidget* parent)
 : QMainWindow(parent)
 , myGuiPtr(new Ui::MyTMSUUI_MainWindow)
 , myDataPtr(nullptr)
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

   //// Check Box: Recurse
   connect(myGuiPtr->myRecurseCheckbox, SIGNAL(   stateChanged(int)),
                                  this,   SLOT(doUpdateRecurse(int)) );

   //// Radio Button: Query
   connect(myGuiPtr->myQueryRadioButton, SIGNAL(          clicked()),
                                   this,   SLOT(radioQueryClicked()) );

   //// Radio Button: Set tags
   connect(myGuiPtr->mySetTagsRadioButton, SIGNAL(            clicked()),
                                     this,   SLOT(radioSetTagsClicked()) );

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

      //// I/f: Gone idle
      connect(&(myDataPtr->myInterface), SIGNAL(         goneIdle(MyTMSUUI_IF_NS::ProcState, bool)),
                                   this,   SLOT(interfaceGoneIdle(MyTMSUUI_IF_NS::ProcState, bool)) );

      emit dataBaseDirChanged(myDataPtr->myCurrentBaseDir.absolutePath());
   }
}

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
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::rebuildTagWidgets()
{
   //// Clear out any old widgets
   clearTagWidgets();

   size_t widgetNum = 0;

   MyTMSUUI_TagWidget* newTagWidget;
   QVBoxLayout* tagWidgetsVLayout = new QVBoxLayout;

   //// Iterate over list of tags from Data Ptr
   for (MyTMSUUI_TagData* tagData : myDataPtr->myTagsList)
   {
      newTagWidget = new MyTMSUUI_TagWidget(myGuiPtr->myTagsParentWidget);
      newTagWidget->setObjectName(QString("tagWidget") + QString::number(widgetNum++));

      newTagWidget->configure(tagData);

      tagWidgetsVLayout->addWidget(newTagWidget);
   }

   myGuiPtr->myTagsParentWidget->setLayout(tagWidgetsVLayout);
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::prepFilesListForDisplay()
{
   if (myDataPtr == nullptr)
   {
      qCritical("null data object (how did this happen?)");
      return;
   }

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
void MyTMSUUI_MainWindow::beginDisplayList()
{
   if (myDataPtr == nullptr)
   {
      qCritical("null data object (how did this happen?)");
      return;
   }

   myGuiPtr->myStatusBar->clearMessage();

   if (myDataPtr->myCurrentFilesList.isEmpty())
   {
      myGuiStatusBarNormalLabel->setText("");
      myGuiStatusBarErrorLabel->setText("No image files in current base directory");
      return;
   }
   //// else

   goToImage(1);

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::goToImage(qsizetype number) //// TODO 
{
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::goToLastImage() //// TODO 
{
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::doSelectBaseDir()
{
   if (myDataPtr == nullptr)
   {
      statusBar()->showMessage("INTERNAL ERROR: Missing data object - cannot set a base directory");
      return;
   }

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
void MyTMSUUI_MainWindow::firstButtonClicked() //// TODO
{
   qDebug("TODO firstButtonClicked");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::prevButtonClicked() //// TODO
{
   qDebug("TODO prevButtonClicked");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::nextButtonClicked() //// TODO
{
   qDebug("TODO nextButtonClicked");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::lastButtonClicked() //// TODO
{
   qDebug("TODO lastButtonClicked");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::applyButtonClicked() //// TODO
{
   qDebug("TODO applyButtonClicked");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::doUpdateRecurse(int newRecurseState)
{
   if (myDataPtr == nullptr)
   {
      statusBar()->showMessage("INTERNAL ERROR: Missing data object - cannot set the 'recurse enabled' flag");
      return;
   }

   myDataPtr->myRecurseEnabled = (newRecurseState == Qt::Checked);
   myDataPtr->myInterface.retrieveFilesList(false); //// TODO: determine if any tags selected for query
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioQueryClicked() //// TODO
{
   myGuiPtr->myRetrieveOptionsGroupBox->setEnabled(true);
   qDebug("TODO radioQueryClicked");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioSetTagsClicked() //// TODO
{
   myGuiPtr->myRetrieveOptionsGroupBox->setEnabled(false);
   qDebug("TODO radioSetTagsClicked");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioAllClicked() //// TODO
{
   qDebug("TODO radioAllClicked");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioNoneClicked() //// TODO
{
   qDebug("TODO radioNoneClicked");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::setStatusUpdating()
{
   myGuiPtr->myStatusBar->showMessage("Updating...");
   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_MainWindow::interfaceGoneIdle(MyTMSUUI_IF_NS::ProcState lastState, bool withError)
{
   myGuiPtr->myStatusBar->clearMessage();

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
            //// TODO: Clear file list, etc?
         }
         else
         {
            //// Update list of image files based on new directory
            setStatusUpdating();
            myDataPtr->myInterface.retrieveFilesList(false); //// TODO: determine if any tags selected for query
         }

         break;

      case MyTMSUUI_IF_NS::ImpliesDBQuery:
         if (withError)
            break;

         //// (Re-)build tag widgets
         setStatusUpdating();
         rebuildTagWidgets();

         //// Update list of image files based on new directory/database
         myDataPtr->myInterface.retrieveFilesList(false);
         break;

      case MyTMSUUI_IF_NS::BuildFilesList:
         if (withError)
         {
            //// TODO: Clear file list, etc?
         }
         else
         {
            //// Update widgets for new files list
            setStatusUpdating();
            prepFilesListForDisplay();
            beginDisplayList();
         }

         break;

      default:
         break;
   }

   return;
}

