#include "mytmsuui_mainwindow.h"
#include "ui_mytmsuui_mainwindow.h"
#include "mytmsuui_data.h"
#include <stdio.h> // TODO: remove

// ----------------------------------------------------------------------------
MyTMSUUI_MainWindow::MyTMSUUI_MainWindow(QWidget* parent)
 : QMainWindow(parent)
 , myGuiPtr(new Ui::MyTMSUUI_MainWindow)
 , myDataPtr(nullptr)
{
   // Setup the UI widgets (based on the Designer "ui" file)
   myGuiPtr->setupUi(this);

   // -----------------
   // Setup Connections
   // -----------------
   { // Block for folding

   // Action: Quit
   connect(myGuiPtr->myQuitAction, SIGNAL(triggered()),
                             this,   SLOT(close())
          );

   // Action: Select base dir
   connect(myGuiPtr->mySelectBaseDirAction, SIGNAL(triggered()),
                                      this,   SLOT(doSelectBaseDir())
          );

   // Push Button: Select base dir
   connect(myGuiPtr->mySelectBaseDirBtn, SIGNAL(clicked()),
                                   this,   SLOT(doSelectBaseDir())
          );

   // Push Button: First
   connect(myGuiPtr->myFirstImgButton, SIGNAL(clicked()),
                                 this,   SLOT(firstButtonClicked())
          );

   // Push Button: Prev
   connect(myGuiPtr->myPrevImgButton, SIGNAL(clicked()),
                                this,   SLOT(prevButtonClicked())
          );

   // Push Button: Next
   connect(myGuiPtr->myNextImgButton, SIGNAL(clicked()),
                                this,   SLOT(nextButtonClicked())
          );

   // Push Button: Last
   connect(myGuiPtr->myLastImgButton, SIGNAL(clicked()),
                                this,   SLOT(lastButtonClicked())
          );

   // Push Button: Apply
   connect(myGuiPtr->myApplyButton, SIGNAL(clicked()),
                              this,   SLOT(applyButtonClicked())
          );

   // Check Box: Recurse
   connect(myGuiPtr->myRecurseCheckbox, SIGNAL(stateChanged(int)),
                                  this,   SLOT(doUpdateRecurse(int))
          );

   // Radio Button: Query
   connect(myGuiPtr->myQueryRadioButton, SIGNAL(clicked()),
                                   this,   SLOT(radioClicked())
          );

   // Radio Button: Set tags
   connect(myGuiPtr->mySetTagsRadioButton, SIGNAL(clicked()),
                                     this,   SLOT(radioClicked())
          );

   } // Block for folding

}

// ----------------------------------------------------------------------------
MyTMSUUI_MainWindow::~MyTMSUUI_MainWindow()
{
   delete myGuiPtr;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_MainWindow::doSelectBaseDir()
{
   printf("TODO doSelectBaseDir\n");
   // if (myDataPtr != nullptr)
   // {
   //    printf("  Current Base Dir = %s\n", myDataPtr->getCurrentBaseDir().absolutePath().toStdString().c_str());
   // }

   return;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_MainWindow::firstButtonClicked()
{
   printf("TODO firstButtonClicked\n");
   return;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_MainWindow::prevButtonClicked()
{
   printf("TODO prevButtonClicked\n");
   return;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_MainWindow::nextButtonClicked()
{
   printf("TODO nextButtonClicked\n");
   return;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_MainWindow::lastButtonClicked()
{
   printf("TODO lastButtonClicked\n");
   return;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_MainWindow::applyButtonClicked()
{
   printf("TODO applyButtonClicked\n");
   return;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_MainWindow::doUpdateRecurse(int newRecurseState)
{
   printf("TODO doUpdateRecurse %d\n", newRecurseState);
   return;
}

// ----------------------------------------------------------------------------
void MyTMSUUI_MainWindow::radioClicked()
{
   printf("TODO radioClicked\n");
   return;
}

