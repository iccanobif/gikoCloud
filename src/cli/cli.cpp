#include "cli.hpp"
#include <stdio.h>
#include "../net/connection.hpp"

ThingsDoer doer;
CPConnection conn;

void ThingsDoer::onHandshaken()
{
    printf("doing stuff\n");
    conn.connectToServer(CPConnection::Foreign, "Red");
}
void ThingsDoer::onclientIdReceived(quint32 clientId)
{
    printf("clientidreceived\n");
    this->clientId = clientId;
}
void ThingsDoer::onloginCountChanged(quint32 loginCount)
{
    printf("onloginCountChanged\n");
    this->loginCount = loginCount;
}
void ThingsDoer::ontripcodeReceived(const QByteArray &tripcode)
{
    printf("ontripcodeReceived\n");
}
void ThingsDoer::onerror(const QString &str)
{
    printf("onerror\n");
    printf("%s", str.toUtf8().constData());
}
void ThingsDoer::onserverResponse(bool isResult, const QString &command)
{
    printf("onserverResponse\n");
    // Connected
    if (isResult == true)
    {
        conn.sendPassword("");
    }
    else
    {
        this->ondisconnected();
    }
}
void ThingsDoer::onloginDetailsRequested()
{
    printf("onloginDetailsRequested\n");
    // Uncomment this to use tripcodes

    // if (hasTripbase())
    // {
    //     conn.sendTripcodeBase(tripbase());
    // }
    // else
    // {
    conn.sendHash();
    // }
}
void ThingsDoer::onwaitingForStageEntry()
{
    printf("onwaitingForStageEntry\n");
    conn.enterStage("jinja", CPSharedObject::Giko);
}
void ThingsDoer::stageEntrySuccessful()
{
    printf("Login completed\n");
}
void ThingsDoer::ondisconnected()
{
    printf("ondisconnected\n");
}

int main()
{
    printf("Starting...\n");

    QObject::connect(&conn, SIGNAL(handshaken()), &doer, SLOT(onHandshaken()));

    QObject::connect(&conn, SIGNAL(clientIdReceived(quint32)), &doer, SLOT(onclientIdReceived(quint32)));
    QObject::connect(&conn, SIGNAL(loginCountChanged(quint32)), &doer, SLOT(onloginCountChanged(quint32)));
    QObject::connect(&conn, SIGNAL(tripcodeReceived(const QByteArray)), &doer, SLOT(ontripcodeReceived(const QByteArray)));

    QObject::connect(&conn, SIGNAL(error(const QString)), &doer, SLOT(onerror(const QString)));
    QObject::connect(&conn, SIGNAL(serverResponse(bool, const QString)), &doer, SLOT(onserverResponse(bool, const QString)));
    QObject::connect(&conn, SIGNAL(loginDetailsRequested()), &doer, SLOT(onloginDetailsRequested()));
    QObject::connect(&conn, SIGNAL(waitingForStageEntry()), &doer, SLOT(onwaitingForStageEntry()));
    QObject::connect(&conn, SIGNAL(stageEntrySuccessful()), &doer, SLOT(stageEntrySuccessful()));

    QObject::connect(&conn, SIGNAL(disconnected()), &doer, SLOT(ondisconnected()));

    // TODO agganciare tutti i segnali di CPConnection
    conn.connectToHost();
    return 0;
}