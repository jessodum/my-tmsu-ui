#include "mytmsuui_tagwidget.h"
#include "ui_mytmsuui_tagwidget.h"
#include "mytmsuui_tagdata.h"
#include <QtLogging>

//// --------------------------------------------------------------------------
MyTMSUUI_TagWidget::MyTMSUUI_TagWidget(QWidget* parent)
 : QWidget(parent)
 , myGuiPtr(new Ui::MyTMSUUI_TagWidget)
 , myCheckedState(MyTMSUUI_Tagged_NS::Unchecked)
 , myToggledByUserClick(true)
{
   //// Setup the UI widgets (based on the Designer "ui" file)
   myGuiPtr->setupUi(this);

   //// -----------------
   //// Setup Connections
   //// -----------------

   //// Check Box
   connect(myGuiPtr->myTagCheckbox, SIGNAL(          clicked()),
                              this,   SLOT(doCheckboxClicked()) );
   connect(myGuiPtr->myTagCheckbox, SIGNAL(          toggled(bool)),
                              this,   SLOT(doCheckboxToggled()) );
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

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::setCheckedState(MyTMSUUI_Tagged_NS::CheckedState state, bool toggleIfNeeded)
{
   switch (state)
   {
      case MyTMSUUI_Tagged_NS::SetExplicitTag:
      case MyTMSUUI_Tagged_NS::SetImpliedTag:
      case MyTMSUUI_Tagged_NS::SetBothTag:
      case MyTMSUUI_Tagged_NS::ToBeSetExplicitTag:
      case MyTMSUUI_Tagged_NS::ToBeSetImpliedTag:
      case MyTMSUUI_Tagged_NS::ToBeSetBothTag:
         if (isChecked())
            break;
         //// else

         if (toggleIfNeeded)
         {
            myToggledByUserClick = false;
            myGuiPtr->myTagCheckbox->setChecked(true);
         }
         else
         {
            qCritical("Attempting to set a checked state (%d) for an unchecked widget without toggling allowed", state);
            return;
         }

         break;

      case MyTMSUUI_Tagged_NS::Unchecked:
      case MyTMSUUI_Tagged_NS::ToBeUnchecked:
         if (!isChecked())
            break;
         //// else

         if (toggleIfNeeded)
         {
            myToggledByUserClick = false;
            myGuiPtr->myTagCheckbox->setChecked(false);
         }
         else
         {
            qCritical("Attempting to set an unchecked state (%d) for a checked widget without toggling allowed", state);
            return;
         }

         break;

      default:
         qCritical("Unhandled state: %d", state);
         return;
   }

   if (myCheckedState != state)
   {
      myCheckedState = state;
   }

   updateCheckboxStyle();

   return;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::resetUserClicked()
{
   myToggledByUserClick = true;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::updateCheckboxStyle()
{
   QString newStyleString;
   switch (myCheckedState)
   {
      case MyTMSUUI_Tagged_NS::SetExplicitTag:
         newStyleString = "QCheckBox::indicator:checked {image: url(:cb/img/cb_checked_blue.png);}";
         break;

      case MyTMSUUI_Tagged_NS::SetImpliedTag:
         newStyleString = "QCheckBox::indicator:checked {image: url(:cb/img/cb_checked_purple.png);}";
         break;

      case MyTMSUUI_Tagged_NS::SetBothTag:
         newStyleString = "QCheckBox::indicator:checked {image: url(:cb/img/cb_checked_orange.png);}";
         break;

      case MyTMSUUI_Tagged_NS::ToBeSetExplicitTag:
         newStyleString = "QCheckBox::indicator:checked {image: url(:cb/img/cb_checked_green.png);}";
         break;

      case MyTMSUUI_Tagged_NS::ToBeSetImpliedTag:
         newStyleString = "QCheckBox::indicator:checked {image: url(:cb/img/cb_checked_pink.png);}";
         break;

      case MyTMSUUI_Tagged_NS::ToBeSetBothTag:
         newStyleString = "QCheckBox::indicator:checked {image: url(:cb/img/cb_checked_yellow.png);}";
         break;

      case MyTMSUUI_Tagged_NS::Unchecked:
         newStyleString = "QCheckBox::indicator:unchecked {image: url(:cb/img/cb_unchecked_grey.png);}";
         break;

      case MyTMSUUI_Tagged_NS::ToBeUnchecked:
         newStyleString = "QCheckBox::indicator:unchecked {image: url(:cb/img/cb_unchecked_red.png);}";
         break;

      default:
         qCritical("Unhandled state: %d", myCheckedState);
         break;
   }

   if (!newStyleString.isEmpty())
   {
      myGuiPtr->myTagCheckbox->setStyleSheet(newStyleString);
   }
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::doCheckboxClicked()
{
   emit tagToggled(getTagName(), myToggledByUserClick);
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::doCheckboxToggled()
{
   if (!myToggledByUserClick)
   {
      emit tagToggled(getTagName(), myToggledByUserClick);
   }
   //// else, defer to "clicked" signal-slot
}

