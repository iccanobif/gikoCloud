#ifndef CLI_PLAYERINFO
#define CLI_PLAYERINFO

#include <QString>

class PlayerInfo
{
public:
  PlayerInfo()
  {
    // Apparently building will fail if I declare somewhere a map<key, T> where T is 
    // a class without a constructor with zero parameters. Why?
  }
  PlayerInfo(const QString &username, int x, int y)
  {
    this->username = username;
    this->x = x;
    this->y = y;
  }
  QString username;
  int x;
  int y;
};

#endif // CLI_PLAYERINFO
