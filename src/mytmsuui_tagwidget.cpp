#include "mytmsuui_tagwidget.h"
#include "ui_mytmsuui_tagwidget.h"
#include "mytmsuui_tagdata.h"

//// --------------------------------------------------------------------------
MyTMSUUI_TagWidget::MyTMSUUI_TagWidget(QWidget* parent)
 : QWidget(parent)
 , myGuiPtr(new Ui::MyTMSUUI_TagWidget)
{
   //// Setup the UI widgets (based on the Designer "ui" file)
   myGuiPtr->setupUi(this);

   //// -----------------
   //// TODO: Setup Connections
   //// -----------------
}

//// --------------------------------------------------------------------------
MyTMSUUI_TagWidget::~MyTMSUUI_TagWidget()
{
   delete myGuiPtr;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::configure(MyTMSUUI_TagData* tagData)
{
   myGuiPtr->myTagCheckbox->setText(tagData->getTagName());

   if (tagData->getValuesList().size() > 0)
   {
      myGuiPtr->myValueSelectBox->setEnabled(true);
      myGuiPtr->myValueSelectBox->addItems(tagData->getValuesList());
   }
   else
   {
      myGuiPtr->myEqualsValueLabel->hide();
      myGuiPtr->myValueSelectBox->hide();
   }
}

