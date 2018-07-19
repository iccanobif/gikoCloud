#include "connectionwrapper.hpp"

ConnectionWrapper::ConnectionWrapper(QObject *parent, 
                                     CPConnection *conn,
                                     CliParameters *cliParameters) : QObject(parent)
{
    this->conn = conn;
    this->cliParameters = cliParameters;
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
    conn->connectToServer(this->cliParameters->server, this->cliParameters->username);
}
void ConnectionWrapper::onclientIdReceived(quint32 clientId)
{
    fprintf(stderr, "clientidreceived: %d\n", clientId);
    this->clientId = clientId;
}
void ConnectionWrapper::onerror(const QString &str)
{
    fprintf(stderr, "onerror: %s\n", str.toUtf8().constData());
    exit(-1);
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
    conn->enterStage(this->cliParameters->roomId, this->cliParameters->character);
}
void ConnectionWrapper::stageEntrySuccessful()
{
    fprintf(stderr, "Login completed\n");
    emit connectionCompleted(this->clientId);
}

void ConnectionWrapper::ondisconnected()
{
    fprintf(stderr, "ondisconnected\n");
}