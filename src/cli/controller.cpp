#include <stdlib.h>
#include <unistd.h>
#include <QThread>
#include "controller.hpp"

Controller::Controller(QObject *parent)
{
    fprintf(stderr, "Starting...\n");
    conn = new CPConnection(parent);
    // conn.setProxy("184.178.172.18", 15280);
    connectionWrapper = new ConnectionWrapper(parent, conn);

    QObject::connect(connectionWrapper, &ConnectionWrapper::connectionCompleted, this, &Controller::startCLI);
    connectionWrapper->startConnection();
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
    QObject::connect(conn, &CPConnection::playerMessageReceived, this, &Controller::receiveMessageFromGiko);
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
    else
        fprintf(stderr, "Unrecognized command, sorry.\n");

    free(line);
}

void Controller::receiveMessageFromGiko(quint32 playerId, const QString &message)
{
    //Prints username and message in JSON

    char msgToPrint[10000];
    int msgToPrintLenght = sprintf(msgToPrint, "MSG {\"user\": \"%d\", \"message\": \"%s\"}\n",
                                   playerId,
                                   QString(message).replace("\"", "\\\"").toUtf8().constData());
    // Directly using write() because for reasons I don't understand, printf() doesn't work (won't even call
    // write() as I could see from strace) when this program is run as a child_process from node.js...
    write(1, msgToPrint, msgToPrintLenght);
}
