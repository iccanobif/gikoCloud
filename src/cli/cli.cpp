#include "cli.hpp"
#include <stdio.h>
#include "../net/connection.hpp"
#include <QCoreApplication>
#include <QThread>

void ThingsDoer::onHandshaken()
{
    printf("doing stuff\n");
    conn->connectToServer(CPConnection::Foreign, "Red");
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
    printf("onerror: %s\n", str.toUtf8().constData());
}
void ThingsDoer::onserverResponse(bool isResult, const QString &command)
{
    printf("onserverResponse\n");
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
void ThingsDoer::onloginDetailsRequested()
{
    printf("onloginDetailsRequested\n");
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
void ThingsDoer::onwaitingForStageEntry()
{
    printf("onwaitingForStageEntry\n");
    conn->enterStage("jinja", CPSharedObject::Giko);
}
void ThingsDoer::stageEntrySuccessful()
{
    printf("Login completed\n");
}

void ThingsDoer::ondisconnected()
{
    printf("ondisconnected\n");
}

void ThingsDoer::launchCLI()
{
    emit startCLI();
}

void ThingsDoer::cliSendsMessage(char *message)
{
    printf("Sending message %s...\n", message);
    // conn->sendClientMessage(&message.toUtf8().constData());
    // free(message); // There's a memory leak, here! And this free() doesn't work, i guess because that
    // message was malloc'd in another thread.
}

int main(int argc, char *argv[])
{
    printf("Starting...\n");
    QCoreApplication app(argc, argv);

    CPConnection conn;
    ThingsDoer doer(&app, &conn);

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

    // TODO make another object with its own signals and slots for the CLI

    printf("Dopo le connessioni\n");

    CliThread cliThread;

    QObject::connect(&cliThread, SIGNAL(sendingMessageToGiko(char *)), &doer, SLOT(cliSendsMessage(char *)));
    printf("prima dello start\n");
    cliThread.start();

    // QThread::sleep(5);
    printf("prima del'app.exec()\n");
    return app.exec();
}