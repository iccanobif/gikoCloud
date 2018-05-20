#ifndef CUTIEPOI_NET_STAGE_INFO_HPP
#define CUTIEPOI_NET_STAGE_INFO_HPP

#include <QByteArray>
#include "shared-object.hpp"

class CPStageInfo {
public:
    typedef CPSharedObject::Direction Direction;
private:
    QByteArray m_stageName;
    quint8 m_stageSpeed;
    quint8 m_xPos;
    quint8 m_yPos;
    Direction m_direction;
public:
    CPStageInfo(const QByteArray &name, quint8 xpos, quint8 ypos,
                Direction dir = Direction::Left,
                quint8 stageSpeed = 1);

    static const CPStageInfo *fromStageId(const QString &stageId);

    const static CPStageInfo Shrine;
    const static CPStageInfo ShrineEntrance;
    const static CPStageInfo EelTrack;

    const static CPStageInfo Schoolyard;
    const static CPStageInfo SchoolCrossing;
    const static CPStageInfo SchoolClassroom;
    const static CPStageInfo SchoolHallway;
    const static CPStageInfo SchoolCulturalRoom;
    const static CPStageInfo SchoolComputerLab;

    const static CPStageInfo Bar774;
    const static CPStageInfo DrunkersBar;
    const static CPStageInfo UndergroundTown;

    const static CPStageInfo Bar;
    const static CPStageInfo BarStreet;
    const static CPStageInfo RamenStand;

    const static CPStageInfo AdminStreet;
    const static CPStageInfo AdminsBar;
    const static CPStageInfo DevelopersLounge;

    const static CPStageInfo BanquetingHall;
    const static CPStageInfo CafeVista;
    const static CPStageInfo WellA;
    const static CPStageInfo WellB;

    const static CPStageInfo BusStop;
    const static CPStageInfo Seashore;

    const static CPStageInfo RadioStudio;
    const static CPStageInfo RadioRoom1;
    const static CPStageInfo DressingRoom;
    const static CPStageInfo Backstage;
    const static CPStageInfo BeatGiko;
    const static CPStageInfo GSquid;

    const static CPStageInfo Hilltop;
    const static CPStageInfo HilltopStairway;

    const static CPStageInfo Dogrun;
    const static CPStageInfo Forest;
    const static CPStageInfo Cafe;

    const static CPStageInfo Test1;
    const static CPStageInfo Test2;
    const static CPStageInfo Test3;
    const static CPStageInfo Test4;
    const static CPStageInfo Test5;
    const static CPStageInfo Test6;
    const static CPStageInfo Test7;
    const static CPStageInfo Test8;
    const static CPStageInfo Test9;
    const static CPStageInfo Test10;

    // getters
    inline const QByteArray &name() const Q_DECL_NOTHROW;
    inline quint32 xPos() const Q_DECL_NOTHROW;
    inline quint32 yPos() const Q_DECL_NOTHROW;
    inline quint32 speed() const Q_DECL_NOTHROW;
    inline Direction direction() const Q_DECL_NOTHROW;

    // setters
    inline void setXPos(quint32 xpos) Q_DECL_NOTHROW;
    inline void setYPos(quint32 ypos) Q_DECL_NOTHROW;
};

inline const QByteArray &CPStageInfo::name() const Q_DECL_NOTHROW
{
    return this->m_stageName;
}

inline quint32 CPStageInfo::xPos() const Q_DECL_NOTHROW
{
    return this->m_xPos;
}

inline quint32 CPStageInfo::yPos() const Q_DECL_NOTHROW
{
    return this->m_yPos;
}

inline quint32 CPStageInfo::speed() const Q_DECL_NOTHROW
{
    return this->m_stageSpeed;
}

inline CPStageInfo::Direction CPStageInfo::direction() const Q_DECL_NOTHROW
{
    return this->m_direction;
}

inline void CPStageInfo::setXPos(quint32 xpos) Q_DECL_NOTHROW {
    this->m_xPos = xpos;
}

inline void CPStageInfo::setYPos(quint32 ypos) Q_DECL_NOTHROW {
    this->m_yPos = ypos;
}

#endif // CUTIEPOI_NET_STAGE_INFO_HPP
