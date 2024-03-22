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

 public slots:
   void doNewBaseDir(const QString& newPath);

 protected:

 private:
   QProcess myIFProc;
};

// =======
// INLINES
// =======

inline QProcess* MyTMSUUI_Interface::getIFProcess()
{
   return &myIFProc;
}

#endif
