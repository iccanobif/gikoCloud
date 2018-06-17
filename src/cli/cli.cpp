#include "cli.hpp"
#include <stdio.h>
#include <QCoreApplication>
#include "controller.hpp"
#include "../net/connection.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CliParameters params;
    params.username = QString("kek");
    params.server = CPConnection::Young;

    Controller c(&params, &app);

    fprintf(stderr, "before app.exec()\n");
    QObject::connect(&c, &Controller::quit, &app, &QCoreApplication::quit);
    return app.exec();
}