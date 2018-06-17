#ifndef CLI_CLIPARAMETERS
#define CLI_CLIPARAMETERS

#include <QString>
#include "../net/connection.hpp"
#include "../net/stage-info.hpp"

// This could be a singleton?
class CliParameters
{
    public:
    QString username;
    CPConnection::Server server;
    CPStageInfo room = CPStageInfo::DevelopersLounge;
    CPSharedObject::Character character = CPSharedObject::Boon;
    QString proxyHostname = nullptr;
    int proxyPort = 0;
};

#endif // CLI_CLIPARAMETERS
