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

//// --------------------------------------------------------------------------
bool MyTMSUUI_TagWidget::isChecked() const
{
   return myGuiPtr->myTagCheckbox->isChecked();
}

//// --------------------------------------------------------------------------
QString MyTMSUUI_TagWidget::getTagName() const
{
   return myGuiPtr->myTagCheckbox->text();
}

//// --------------------------------------------------------------------------
QString MyTMSUUI_TagWidget::getValue() const
{
   QString retval("");

   if (myGuiPtr->myValueSelectBox->isEnabled())
   {
      QString currentText = myGuiPtr->myValueSelectBox->currentText();

      retval = (currentText.isEmpty() ? myGuiPtr->myValueSelectBox->placeholderText() : currentText);
   }

   return retval;
}

