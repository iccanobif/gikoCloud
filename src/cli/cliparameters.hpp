#ifndef CLI_CLIPARAMETERS
#define CLI_CLIPARAMETERS

#include <QString>
#include "../net/connection.hpp"
#include "../net/stage-info.hpp"

// This could be a singleton?
class CliParameters
{
  public:
    QString username = QString(" ");
    CPConnection::Server server = CPConnection::Foreign;
    CPStageInfo room = CPStageInfo::GSquid;
    CPSharedObject::Character character = CPSharedObject::Boon;
    QString proxyHostname = nullptr;
    int proxyPort = 0;
    QString parameterSummary()
    {
        return QString("Username: \"") + username + QString("\"")
               + QString("\nServer: ")
               + QString(server == 0 ? "General" :
                         server == 1 ? "Foreign" :
                         server == 2 ? "Young" :
                         server == 3 ? "Vip" :
                         server == 4 ? "Kws" : "dunno")
               + QString("\nRoom: ") + QString(room.name())
               + QString("\nCharacter: ") + QString::number(character)
               + QString("\nProxy: ") + proxyHostname + QString(" port: ") + QString::number(proxyPort);
    }
};

#endif // CLI_CLIPARAMETERS
