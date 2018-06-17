#include "cli.hpp"
#include <stdio.h>
#include <QCoreApplication>
#include "controller.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Controller c(&app);
    fprintf(stderr, "before app.exec()\n");
    QObject::connect(&c, &Controller::quit, &app, &QCoreApplication::quit);
    return app.exec();
}