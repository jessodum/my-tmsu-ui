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
   QString getValuePlaceholderText() const;
   bool usesValues() const;
   MyTMSUUI_TagData* getTagData() const;

   //// Accessors: Set
   void setCheckedState(MyTMSUUI_Tagged_NS::CheckedState state, bool toggleIfNeeded = false);
   void setValue(const QString& value);

 public slots:
   void clickCheckbox();
   void resetUserClicked();
   void resetValuesSelect();
   void doValueSelected(int selectedIndex);

 signals:
   void tagToggled(const QString& tagName, bool byUserClick);
   void shortTagClicked(const QString& tagName, bool byUserClick);
   void valueIndexChanged(const QString& tagName, int index);

 protected:
   void updateCheckboxStyle();
   bool eventFilter(QObject* watchedObj, QEvent* event) override;

 protected slots:
   void doCheckboxClicked();
   void doCheckboxToggled();
   void doValueIdxChanged(int idx);

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
