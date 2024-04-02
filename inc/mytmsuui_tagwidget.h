#ifndef MYTMSUUI_TAGWIDGET_H
#define MYTMSUUI_TAGWIDGET_H

#include "mytmsuui_taggedvalue.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
   class MyTMSUUI_TagWidget;
}
QT_END_NAMESPACE

//// Forward declarations
class MyTMSUUI_TagData;

//// ==========================================================================

class MyTMSUUI_TagWidget : public QWidget
{
 Q_OBJECT

 public:
   MyTMSUUI_TagWidget(QWidget* parent = nullptr);
   ~MyTMSUUI_TagWidget();

   void configure(MyTMSUUI_TagData* tagData);

   //// Accessors: Get
   bool isChecked() const;
   MyTMSUUI_Tagged_NS::CheckedState getCheckedState() const;
   QString getTagName() const;
   QString getValue() const;

   //// Accessors: Set
   void setCheckedState(MyTMSUUI_Tagged_NS::CheckedState state, bool toggleIfNeeded = false);

 public slots:
   void resetUserClicked();

 signals:
   void tagToggled(const QString& tagName, bool byUserClick);

 protected:
   void updateCheckboxStyle();

 protected slots:
   void doCheckboxClicked();
   void doCheckboxToggled();

 private:
   Ui::MyTMSUUI_TagWidget* myGuiPtr;
   MyTMSUUI_Tagged_NS::CheckedState myCheckedState;
   bool myToggledByUserClick;

};

//// =======
//// INLINES
//// =======

inline MyTMSUUI_Tagged_NS::CheckedState MyTMSUUI_TagWidget::getCheckedState() const
{
   return myCheckedState;
}

#endif // MYTMSUUI_TAGWIDGET_H
