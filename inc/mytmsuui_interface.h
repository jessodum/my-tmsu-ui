#ifndef MYTMSUUI_INTERFACE_H
#define MYTMSUUI_INTERFACE_H

#include <QProcess>

namespace MyTMSUUI_IF_NS
{
   enum ProcState
   {
      Idle,
      InfoQuery,
      TagsDBQuery,
      BuildImgFileList
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
   const QString& getError() const;

 signals:
   void goneIdle(MyTMSUUI_IF_NS::ProcState lastState, bool withError = false);

 public slots:
   void doNewBaseDir(const QString& newPath);

 protected:
   void handleFinishedInfoQuery(int exitCode);
   void goIdle(bool withError = false);

 protected slots:
   void handleFinishedProc(int exitCode, QProcess::ExitStatus howExited);

 private:
   QProcess myIFProc;
   MyTMSUUI_IF_NS::ProcState myState;
   QString myErrorStr;
   QString myDBRootPath;
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

inline const QString& MyTMSUUI_Interface::getError() const
{
   return myErrorStr;
}

inline void MyTMSUUI_Interface::goIdle(bool withError)
{
   MyTMSUUI_IF_NS::ProcState prevState = myState;
   emit goneIdle(prevState, withError);
}

#endif
