#ifndef MYTMSUUI_TAGWIDGET_H
#define MYTMSUUI_TAGWIDGET_H

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
   //checkedState getCheckedState() const; //// TODO
   QString getTagName() const;
   QString getValue() const;

 protected:

 private:
   Ui::MyTMSUUI_TagWidget* myGuiPtr;

};

#endif // MYTMSUUI_TAGWIDGET_H
