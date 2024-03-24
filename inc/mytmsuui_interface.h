#ifndef MYTMSUUI_INTERFACE_H
#define MYTMSUUI_INTERFACE_H

#include <QProcess>

// Forward declarations
class MyTMSUUI_Data;

// Public NS
namespace MyTMSUUI_IF_NS
{
   enum ProcState
   {
      Idle,
      InfoQuery,
      TagsDBQuery,
      AllValuesDBQuery,
      TagsByValueDBQuery,
      ImpliesDBQuery,
      BuildImgFileList
   };
}

// =====
// Class
// =====
class MyTMSUUI_Interface : public QObject
{
 Q_OBJECT

 public:
   MyTMSUUI_Interface(QObject* parent = nullptr);
   ~MyTMSUUI_Interface();

   // Accessors: Get
   const QProcess* getIFProcess() const;
   MyTMSUUI_IF_NS::ProcState getState() const;
   const QString& getError() const;

   // Accessors: Set
   void setDataObj(MyTMSUUI_Data* dataPtr);

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
   MyTMSUUI_Data* myDataPtr;
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

inline void MyTMSUUI_Interface::setDataObj(MyTMSUUI_Data* dataPtr)
{
   myDataPtr = dataPtr;
}

inline void MyTMSUUI_Interface::goIdle(bool withError)
{
   MyTMSUUI_IF_NS::ProcState prevState = myState;
   emit goneIdle(prevState, withError);
}

#endif
