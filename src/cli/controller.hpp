#ifndef CLI_CONTROLLER
#define CLI_CONTROLLER

#include <QObject>
#include <QSocketNotifier>
#include "../net/connection.hpp"
#include "connectionwrapper.hpp"
#include "cliparameters.hpp"

class Controller : public QObject
{
  Q_OBJECT
  CPConnection *conn;
  ConnectionWrapper *connectionWrapper;
  QSocketNotifier *stdinNotifier;
  CliParameters *cliParameters;

public:
  Controller(CliParameters *cliParameters, QObject *parent = 0);
  ~Controller();
public slots:
  void startCLI();
  void readCommand();
  void receiveMessageFromGiko(quint32 playerId, const QString &message);
signals:
  void sendMessageToGiko(const QString &msg);
  void quit();
};

#endif // CLI_CONTROLLER
