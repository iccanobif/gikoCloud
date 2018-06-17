#ifndef CLI_CLIPARAMETERS
#define CLI_CLIPARAMETERS

#include <QString>
#include "../net/connection.hpp"

// This could be a singleton?
class CliParameters
{
    public:
    QString username;
    CPConnection::Server server;
};

#endif // CLI_CLIPARAMETERS
