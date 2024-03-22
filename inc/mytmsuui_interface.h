#ifndef MYTMSUUI_INTERFACE_H
#define MYTMSUUI_INTERFACE_H

#include <QProcess>

class MyTMSUUI_Interface : public QObject
{
 Q_OBJECT

 public:
   MyTMSUUI_Interface(QObject* parent = nullptr);
   ~MyTMSUUI_Interface();

   QProcess* getIFProcess();

 protected:

 private:
   QProcess myIFProc;
};

inline QProcess* MyTMSUUI_Interface::getIFProcess()
{
   return &myIFProc;
}

#endif
