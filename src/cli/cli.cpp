#include "cli.hpp"
#include <stdio.h>
#include "../net/connection.hpp"
#include <QCoreApplication>
#include <QThread>

ConnectionWrapper::ConnectionWrapper(QObject *parent, CPConnection *conn) : QObject(parent)
{
    this->conn = conn;
    QObject::connect(conn, &CPConnection::handshaken, this, &ConnectionWrapper::onHandshaken);
    QObject::connect(conn, &CPConnection::clientIdReceived, this, &ConnectionWrapper::onclientIdReceived);
    QObject::connect(conn, &CPConnection::error, this, &ConnectionWrapper::onerror);
    QObject::connect(conn, &CPConnection::serverResponse, this, &ConnectionWrapper::onserverResponse);
    QObject::connect(conn, &CPConnection::loginDetailsRequested, this, &ConnectionWrapper::onloginDetailsRequested);
    QObject::connect(conn, &CPConnection::waitingForStageEntry, this, &ConnectionWrapper::onwaitingForStageEntry);
    QObject::connect(conn, &CPConnection::stageEntrySuccessful, this, &ConnectionWrapper::stageEntrySuccessful);
    QObject::connect(conn, &CPConnection::disconnected, this, &ConnectionWrapper::ondisconnected);
}

void ConnectionWrapper::startConnection()
{
    fprintf(stderr, "startConnection()\n");
    conn->connectToHost();
}

void ConnectionWrapper::onHandshaken()
{
    fprintf(stderr, "onHandshaken\n");
    conn->connectToServer(CPConnection::Young, "kek");
}
void ConnectionWrapper::onclientIdReceived(quint32 clientId)
{
    fprintf(stderr, "clientidreceived\n");
    this->clientId = clientId;
}
void ConnectionWrapper::onerror(const QString &str)
{
    fprintf(stderr, "onerror: %s\n", str.toUtf8().constData());
}
void ConnectionWrapper::onserverResponse(bool isResult, const QString &command)
{
    fprintf(stderr, "onserverResponse\n");

    // Connected
    if (isResult == true)
    {
        conn->sendPassword("");
    }
    else
    {
        this->ondisconnected();
    }
}
void ConnectionWrapper::onloginDetailsRequested()
{
    fprintf(stderr, "onloginDetailsRequested\n");
    // Uncomment this to use tripcodes

    // if (hasTripbase())
    // {
    //     conn->sendTripcodeBase(tripbase());
    // }
    // else
    // {
    conn->sendHash();
    // }
}
void ConnectionWrapper::onwaitingForStageEntry()
{
    fprintf(stderr, "onwaitingForStageEntry\n");
    conn->enterStage("school_st", CPSharedObject::Giko);
}
void ConnectionWrapper::stageEntrySuccessful()
{
    fprintf(stderr, "Login completed\n");
    emit connectionCompleted();
}

void ConnectionWrapper::ondisconnected()
{
    fprintf(stderr, "ondisconnected\n");
}

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

    stdinNotifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);

    QObject::connect(this, &Controller::sendMessageToGiko, conn, &CPConnection::sendClientMessage);
    QObject::connect(stdinNotifier, &QSocketNotifier::activated, this, &Controller::readCommand);
}

void Controller::readCommand()
{
    // Maybe this should be able to read more than one line, if there's more than one available?
    char *line = nullptr;
    size_t n = 0;

    ssize_t lineLenght = getline(&line, &n, stdin);
    line[lineLenght - 1] = '\0';

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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Controller c(&app);
    fprintf(stderr, "before app.exec()\n");
    return app.exec();
}