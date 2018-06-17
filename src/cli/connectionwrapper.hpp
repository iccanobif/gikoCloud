#ifndef CLI_CONNECTIONWRAPPER
#define CLI_CONNECTIONWRAPPER

#include <QObject>
#include "../net/connection.hpp"
#include "cliparameters.hpp"

class ConnectionWrapper : public QObject
{
  // The idea of this class is to make a single entry point to start a connection,
  // handling the series of signals sent by CPConnection.
  Q_OBJECT

  CliParameters *cliParameters;

public:
  ConnectionWrapper(QObject *parent, 
                    CPConnection *conn, 
                    CliParameters *cliParameters);
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

#endif // CLI_CONNECTIONWRAPPER
