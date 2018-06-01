#include <QObject>
#include <QThread>
#include "../net/connection.hpp"
#include <stdio.h>
#include <stdlib.h>

class ThingsDoer : public QObject
{
  Q_OBJECT
public:
  ThingsDoer(QObject *parent = 0, CPConnection *conn = 0) : QObject(parent) {
    this->conn = conn;
  }
  void launchCLI();
public slots:
  void startConnection();
  void onHandshaken();
  void onclientIdReceived(quint32 clientId);
  void onloginCountChanged(quint32 loginCount);
  void ontripcodeReceived(const QByteArray &tripcode);
  void onerror(const QString &str);
  void onserverResponse(bool isResult, const QString &command);
  void onloginDetailsRequested();
  void onwaitingForStageEntry();
  void stageEntrySuccessful();
  void ondisconnected();

  void cliSendsMessage(char *message);

signals:
  void startCLI();

private:
  quint32 clientId;
  quint32 loginCount;
  CPConnection *conn;
};

class CliThread : public QThread
{
  Q_OBJECT
public slots:
  void run() override
  {
    char *line = nullptr;
    size_t n = 0;

    while (1)
    {
      ssize_t lineLenght = getline(&line, &n, stdin);
      line[lineLenght-1] = '\0';

      if (!strcmp(line, "connect"))
      {
        printf("Trying to connect...\n");
        char *message = line + 4;
        emit startingConnection();
      }
      else if (!strncmp(line, "msg ", 4))
      {
        char *message = line + 4;
        emit sendingMessageToGiko(message);
      }
      else
        printf("Unrecognized command, sorry.\n");
    }
  }
signals:
  void sendingMessageToGiko(char *);
  void startingConnection();
};