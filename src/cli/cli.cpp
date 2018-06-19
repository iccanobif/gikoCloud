#include "cli.hpp"
#include <stdio.h>
#include <QCoreApplication>
#include "controller.hpp"
#include "../net/connection.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CliParameters params;

    for (int i = 1; i < argc; i++)
    {
        // USER
        if (strcmp(argv[i], "-u") == 0)
        {
            if (i == argc - 1)
            {
                fprintf(stderr, "User not specified.\n");
                return -1;
            }
            params.username = QString(argv[++i]);
        }
        // SERVER
        else if (strcmp(argv[i], "-s") == 0)
        {
            if (i == argc - 1)
            {
                fprintf(stderr, "Server not specified.\n");
                return -1;
            }

            i++;
            if (strcmp(argv[i], "_for") == 0)
                params.server = CPConnection::Foreign;
            else if (strcmp(argv[i], "_gen") == 0)
                params.server = CPConnection::General;
            else if (strcmp(argv[i], "_yng") == 0)
                params.server = CPConnection::Young;
            else if (strcmp(argv[i], "_vip") == 0)
                params.server = CPConnection::Vip;
            else if (strcmp(argv[i], "_kws") == 0)
                params.server = CPConnection::Kws;
            else
            {
                fprintf(stderr, "Server %s not recognized.\n", argv[i]);
                return -1;
            }
        }
        // PROXY
        else if (strcmp(argv[i], "-p") == 0)
        {
            QString par(argv[++i]);
            QStringList splits = par.split(":");
            if (splits.count() != 2)
            {
                fprintf(stderr, "Proxy %s is not in the format hostname:port.\n", argv[i]);
                return -1;
            }
            bool portConvertedOk = false;
            int port = splits[1].toInt(&portConvertedOk);
            if (!portConvertedOk)
            {
                fprintf(stderr, "Port %s is not valid.\n", splits[1].toUtf8().constData());
                return -1;
            }

            params.proxyHostname = splits[0];
            params.proxyPort = port;
        }
        else
        {
            fprintf(stderr, "Parameter %s not recognized.\n", argv[i]);
            return -1;
        }
    }

    fprintf(stderr, "Starting with these settings:\n");
    fprintf(stderr, "%s\n", params.parameterSummary().toUtf8().constData());

    Controller c(&params, &app);

    fprintf(stderr, "before app.exec()\n");
    QObject::connect(&c, &Controller::quit, &app, &QCoreApplication::quit);
    return app.exec();
}