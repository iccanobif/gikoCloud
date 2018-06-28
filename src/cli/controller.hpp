#ifndef CLI_CONTROLLER
#define CLI_CONTROLLER

#include <map>
#include <QObject>
#include <QSocketNotifier>
#include "../net/connection.hpp"
#include "connectionwrapper.hpp"
#include "cliparameters.hpp"
#include "playerinfo.hpp"

class Controller : public QObject
{
  Q_OBJECT
  CPConnection *conn;
  ConnectionWrapper *connectionWrapper;
  QSocketNotifier *stdinNotifier;
  CliParameters *cliParameters;
  std::map<quint32, PlayerInfo> playerInfoMap;
  // canMove: set to false after I sent the server a request for moving the character and before I got back
  // a confirmation of the new position
  bool canMove = false; 
  quint32 thisPlayerId;

public:
  Controller(CliParameters *cliParameters, QObject *parent = 0);
  ~Controller();
public slots:
  void startCLI(quint32 playerId);
  void readCommand();
  void receiveMessageFromGiko(quint32 playerId, const QString &message);
  void receivePlayerName(quint32 playerId, const QString &playerName);
  void receivePlayerPosition(quint32 playerId, int xPos, int yPos);
  void receivePlayerDirection(quint32 playerId, CPSharedObject::Direction direction);
signals:
  void sendMessageToGiko(const QString &msg);
  void sendNewPositionToGiko(int xPos, int yPos, CPSharedObject::Direction direction);
  void sendNewDirectionToGiko(int xPos, int yPos, CPSharedObject::Direction direction);
  void quit();
};

#endif // CLI_CONTROLLER
