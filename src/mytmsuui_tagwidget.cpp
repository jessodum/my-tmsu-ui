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

   //// Install event filter (to filter out mouse wheel events) on the ComboBox.
   //// (Note: "this" class overrides the eventFilter() function; thus we become
   ////  an event filter object).
   myGuiPtr->myValueSelectBox->installEventFilter(this);

   //// -----------------
   //// Setup Connections
   //// -----------------

   //// Check Box
   connect(myGuiPtr->myTagCheckbox, SIGNAL(          clicked()),
                              this,   SLOT(doCheckboxClicked()) );
   connect(myGuiPtr->myTagCheckbox, SIGNAL(          toggled(bool)),
                              this,   SLOT(doCheckboxToggled()) );
   //// Combo Box
   connect(myGuiPtr->myValueSelectBox, SIGNAL(      activated(int)),
                                 this,   SLOT(doValueSelected(int)) );
   connect(myGuiPtr->myValueSelectBox, SIGNAL(currentIndexChanged(int)),
                                 this,   SLOT(  doValueIdxChanged(int)) );
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
   QString retval;

   if (usesValues())
   {
      QString currentText = myGuiPtr->myValueSelectBox->currentText();

      retval = (currentText.isEmpty() ? myGuiPtr->myValueSelectBox->placeholderText() : currentText);
   }

   return retval;
}

//// --------------------------------------------------------------------------
QString MyTMSUUI_TagWidget::getValuePlaceholderText() const
{
   return usesValues() ? myGuiPtr->myValueSelectBox->placeholderText() : "";
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::setValue(const QString& value)
{
   if (usesValues())
   {
      myGuiPtr->myValueSelectBox->setCurrentText(value);
   }
   // else
   // {
   //    qWarning("Attempt to set a value on tag widget that doesn't use values is invalid");
   // }
}

//// --------------------------------------------------------------------------
bool MyTMSUUI_TagWidget::usesValues() const
{
   return myGuiPtr->myValueSelectBox->isEnabled();
}

//// --------------------------------------------------------------------------
MyTMSUUI_TagData* MyTMSUUI_TagWidget::getTagData() const
{
   MyTMSUUI_TagData* retval = new MyTMSUUI_TagData(getTagName());

   QComboBox* values = myGuiPtr->myValueSelectBox;
   if (usesValues() && values->count() > 0)
   {
      for (int i = 0; i < values->count(); ++i)
      {
         retval->addTagValue(values->itemText(i));
      }
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
void MyTMSUUI_TagWidget::clickCheckbox()
{
   myGuiPtr->myTagCheckbox->click();
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::resetUserClicked()
{
   myToggledByUserClick = true;
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::resetValuesSelect()
{
   myGuiPtr->myValueSelectBox->setCurrentIndex(-1);
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::doValueSelected(int selectedIndex)
{
   if ( !isChecked() && selectedIndex >= 0)
   {
      //// Do as if user clicked the checkbox
      myGuiPtr->myTagCheckbox->click();
   }
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
bool MyTMSUUI_TagWidget::eventFilter(QObject* watchedObj, QEvent* event)
{
   if (watchedObj == myGuiPtr->myValueSelectBox)
   {
      //// Ignore ONLY the "wheel" event on the ComboBox.
      if (event->type() == QEvent::Wheel)
      {
         event->ignore(); //// We will not be handling this event, but maybe
                          //// the parent widget will.

         return true; //// Don't let the filter continue processing.
      }
      //// else

      //// Continue on with filtering for all other events.
      return false;
   }
   //// else

   //// Pass the event on to the parent class for any filtering implemented.
   return QWidget::eventFilter(watchedObj, event);
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::doCheckboxClicked()
{
   QWidget* myParent = this->parentWidget();

   if (myParent != nullptr && myParent->objectName() == "myShortTagsParentWidget")
   {
      emit shortTagClicked(getTagName(), myToggledByUserClick);
   }
   else
   {
      emit tagToggled(getTagName(), myToggledByUserClick);
   }
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::doCheckboxToggled()
{
   QWidget* myParent = this->parentWidget();
   bool isShortListTagWidget = (myParent != nullptr && myParent->objectName() == "myShortTagsParentWidget");

   if (!isShortListTagWidget && !myToggledByUserClick)
   {
      emit tagToggled(getTagName(), myToggledByUserClick);
   }
   //// else, defer to "clicked" signal-slot
}

//// --------------------------------------------------------------------------
void MyTMSUUI_TagWidget::doValueIdxChanged(int idx)
{
   emit valueIndexChanged(getTagName(), idx);
}
