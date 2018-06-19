#ifndef CLI_CONTROLLER
#define CLI_CONTROLLER

#include <map>
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
  std::map<quint32, QString> playerNames;

public:
  Controller(CliParameters *cliParameters, QObject *parent = 0);
  ~Controller();
public slots:
  void startCLI();
  void readCommand();
  void receiveMessageFromGiko(quint32 playerId, const QString &message);
  void receivePlayerName(quint32 playerId, const QString &playerName);
signals:
  void sendMessageToGiko(const QString &msg);
  void quit();
};

#endif // CLI_CONTROLLER
