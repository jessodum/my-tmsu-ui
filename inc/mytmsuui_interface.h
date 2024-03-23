#ifndef MYTMSUUI_INTERFACE_H
#define MYTMSUUI_INTERFACE_H

#include <QProcess>

namespace MyTMSUUI_IF_NS
{
   enum ProcState
   {
      Idle,
      InfoQuery
   };
}

class MyTMSUUI_Interface : public QObject
{
 Q_OBJECT

 public:
   MyTMSUUI_Interface(QObject* parent = nullptr);
   ~MyTMSUUI_Interface();

   // Accessors
   const QProcess* getIFProcess() const;
   MyTMSUUI_IF_NS::ProcState getState() const;

 public slots:
   void doNewBaseDir(const QString& newPath);

 protected:
   void handleFinishedInfoQuery(int exitCode);

 protected slots:
   void handleFinishedProc(int exitCode, QProcess::ExitStatus howExited);

 private:
   QProcess myIFProc;
   MyTMSUUI_IF_NS::ProcState myState;
};

// =======
// INLINES
// =======

inline const QProcess* MyTMSUUI_Interface::getIFProcess() const
{
   return &myIFProc;
}

inline MyTMSUUI_IF_NS::ProcState MyTMSUUI_Interface::getState() const
{
	return myState;
}

#endif
