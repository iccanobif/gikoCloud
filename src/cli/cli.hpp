#include <QObject>
#include <QThread>
#include <QSocketNotifier>
#include "../net/connection.hpp"
#include <stdio.h>
#include <stdlib.h>

class ConnectionWrapper : public QObject
{
  // The idea of this class is to make a single entry point to start a connection,
  // handling the series of signals sent by CPConnection.
  Q_OBJECT
public:
  ConnectionWrapper(QObject *parent = 0, CPConnection *conn = 0);
public slots:
  void startConnection();
  void onHandshaken();
  void onclientIdReceived(quint32 clientId);
  void onerror(const QString &str);
  void onserverResponse(bool isResult, const QString &command);
  void onloginDetailsRequested();
  void onwaitingForStageEntry();
  void stageEntrySuccessful();
  void ondisconnected();

signals:
  void connectionCompleted();

private:
  quint32 clientId;
  quint32 loginCount;
  CPConnection *conn;
};

class Controller : public QObject
{
  Q_OBJECT
  CPConnection *conn;
  ConnectionWrapper *connectionWrapper;
  QSocketNotifier *stdinNotifier;

public:
  Controller(QObject *parent = 0);
  ~Controller();
public slots:
  void startCLI();
  void readCommand();
  void receiveMessageFromGiko(quint32 playerId, const QString &message);
signals:
  void sendMessageToGiko(const QString &msg);
  void quit();
};
