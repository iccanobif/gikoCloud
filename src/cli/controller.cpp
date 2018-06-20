#include <stdlib.h>
#include <unistd.h>
#include <QThread>
#include "controller.hpp"
#include "cliparameters.hpp"

Controller::Controller(CliParameters *cliParameters, QObject *parent)
{
    fprintf(stderr, "Starting...\n");
    conn = new CPConnection(parent);
    if (cliParameters->proxyHostname != nullptr)
    {
        conn->setProxy(cliParameters->proxyHostname, cliParameters->proxyPort);
    }

    this->cliParameters = cliParameters;

    connectionWrapper = new ConnectionWrapper(parent, conn, this->cliParameters);

    QObject::connect(connectionWrapper, &ConnectionWrapper::connectionCompleted, this, &Controller::startCLI);
    connectionWrapper->startConnection();

    QObject::connect(conn, &CPConnection::playerNameReceived, this, &Controller::receivePlayerName);
}

Controller::~Controller()
{
}

// Running the connection object in the same thread as the CLI is probably no good:
// while getline() is blocking this thread, the connection object isn't handling incoming messages...
// this might be okay for the initial connection when the CLI isn't accepting commands, but I probably
// have to moveToThread() the connection after login.
void Controller::startCLI()
{
    fprintf(stderr, "startCLI()\n");

    QThread::sleep(5);
    fprintf(stderr, "Finished sleeping.\n");

    stdinNotifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);

    QObject::connect(this, &Controller::sendMessageToGiko, conn, &CPConnection::sendClientMessage);
    QObject::connect(this, &Controller::sendNewPositionToGiko, conn, &CPConnection::sendPosition);
    QObject::connect(conn, &CPConnection::playerMessageReceived, this, &Controller::receiveMessageFromGiko);
    QObject::connect(conn, &CPConnection::playerPositionChanged, this, &Controller::receivePlayerPosition);
    QObject::connect(stdinNotifier, &QSocketNotifier::activated, this, &Controller::readCommand);
}

void trimWhiteSpaceEnd(char *str, int length)
{
    for (int i = length - 1; i >= 0; i--)
    {
        switch (str[i])
        {
        case '\n':
            str[i] = '\0';
            break;
        case '\r':
            str[i] = '\0';
            break;
        case ' ':
            str[i] = '\0';
            break;
        default:
            return;
        }
    }
}

void Controller::readCommand()
{
    // TODO: I can use QTextStream instead.

    // Maybe this should be able to read more than one line, if there's more than one available?
    char *line = nullptr;
    size_t n = 0;

    ssize_t lineLength = getline(&line, &n, stdin);

    if (lineLength == -1)
    {
        fprintf(stderr, "Reached EOF, logging out.\n");
        emit quit(); // Yeah, I know this is leaving memory leaks, I don't give a shit.
        return;
    }

    trimWhiteSpaceEnd(line, lineLength);

    if (!strncmp(line, "msg ", 4))
    {
        char *message = line + 4;
        fprintf(stderr, "Sending message '%s'.\n", message);
        emit sendMessageToGiko(QString::fromUtf8(message));
    }
    else if (!strncmp(line, "msg\0", 4))
    {
        fprintf(stderr, "Sending empty message.\n");
        emit sendMessageToGiko(QString());
    }
    else if (!strncmp(line, "move ", 5))
    {
        QStringList splits = QString::fromUtf8(line).split(QChar(' '));

        if (splits.count() != 4)
        {
            fprintf(stderr, "No good.\n");
            goto END;
        }

        bool convertedOk = false;
        int x = splits.at(1).toInt(&convertedOk);
        if (!convertedOk)
        {
            fprintf(stderr, "%s is not a good number.\n", splits[1].toUtf8().constData());
            goto END;
        }
        int y = splits.at(2).toInt(&convertedOk);
        if (!convertedOk)
        {
            fprintf(stderr, "%s is not a good number.\n", splits[2].toUtf8().constData());
            goto END;
        }
        CPSharedObject::Direction dir;
        if (splits.at(3) == "up")
            dir = CPSharedObject::Up;
        else if (splits.at(3) == "down")
            dir = CPSharedObject::Down;
        else if (splits.at(3) == "left")
            dir = CPSharedObject::Left;
        else if (splits.at(3) == "right")
            dir = CPSharedObject::Right;
        else
            fprintf(stderr, "Direction no good.\n");

        fprintf(stderr, "Movan to %d %d.\n", x, y);
        emit sendNewPositionToGiko(x, y, dir);
    }
    else
        fprintf(stderr, "Unrecognized command, sorry.\n");

END:
    free(line);
}

void Controller::receiveMessageFromGiko(quint32 playerId, const QString &message)
{
    //Prints username and message in JSON

    //TODO just concatenate a series of QStrings and then conver them to constData, no need to use sprintf()
    char msgToPrint[10000];
    int msgToPrintLenght = sprintf(msgToPrint, "MSG {\"playerId\": %d, \"playerName\": \"%s\", \"message\": \"%s\"}\n",
                                   playerId,
                                   playerInfoMap[playerId].username.replace("\"", "\\\"").toUtf8().constData(),
                                   QString(message).replace("\"", "\\\"").toUtf8().constData());
    // Directly using write() because for reasons I don't understand, printf() doesn't work (won't even call
    // write() as I could see from strace) when this program is run as a child_process from node.js...
    write(1, msgToPrint, msgToPrintLenght);
}

void Controller::receivePlayerName(quint32 playerId, const QString &playerName)
{
    char msgToPrint[10000];
    int msgToPrintLenght = sprintf(msgToPrint, "PLAYER_NAME {\"playerId\": %d, \"playerName\": \"%s\"}\n",
                                   playerId,
                                   QString(playerName).replace("\"", "\\\"").toUtf8().constData());
    write(1, msgToPrint, msgToPrintLenght);

    if (playerInfoMap.count(playerId) == 0)
    {
        playerInfoMap[playerId] = PlayerInfo(QString(playerName), 0, 0);
    }
    else
        playerInfoMap[playerId].username = QString(playerName);
}

void Controller::receivePlayerPosition(quint32 playerId, int xPos, int yPos)
{
    char msgToPrint[10000];
    int msgToPrintLenght = sprintf(msgToPrint, "PLAYER_POSITION {\"playerId\": %d, \"x\": %d, \"y\": %d}\n",
                                   playerId,
                                   xPos,
                                   yPos);
    write(1, msgToPrint, msgToPrintLenght);

    if (playerInfoMap.count(playerId) == 0)
    {
        playerInfoMap[playerId] = PlayerInfo(QString(""), xPos, yPos);
    }
    else
    {
        playerInfoMap[playerId].x = xPos;
        playerInfoMap[playerId].y = yPos;
    }
}