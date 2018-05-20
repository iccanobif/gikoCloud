#include "stage-id-hash.hpp"

CPStageInfo::CPStageInfo(const QByteArray &name, quint8 xpos, quint8 ypos, Direction dir,
                         quint8 stageSpeed) :
    m_stageName(name),
    m_stageSpeed(stageSpeed),
    m_xPos(xpos),
    m_yPos(ypos),
    m_direction(dir)
{}

const CPStageInfo CPStageInfo::Shrine("jinja", 12, 6);
const CPStageInfo CPStageInfo::ShrineEntrance("jinja_st", 9, 2);
const CPStageInfo CPStageInfo::EelTrack("long_st", 1, 32, Direction::Up, 2);
const CPStageInfo CPStageInfo::Schoolyard("school_ground", 5, 8, Direction::Up);
const CPStageInfo CPStageInfo::Bar774("bar774", 0, 0);
const CPStageInfo CPStageInfo::DrunkersBar("izakaya774", 0, 0, Direction::Up);
const CPStageInfo CPStageInfo::UndergroundTown("basement", 3, 0, Direction::Down);
const CPStageInfo CPStageInfo::Bar("bar", 8, 4);
const CPStageInfo CPStageInfo::SchoolCrossing("school_st", 0, 2, Direction::Right);
const CPStageInfo CPStageInfo::BarStreet("bar_st", 4, 3, Direction::Down);
const CPStageInfo CPStageInfo::AdminStreet("admin_st", 2, 4, Direction::Down);
const CPStageInfo CPStageInfo::CafeVista("cafe_st", 1, 10);
const CPStageInfo CPStageInfo::BusStop("busstop", 7, 2);
const CPStageInfo CPStageInfo::WellA("idoA", 0, 7, Direction::Right);
const CPStageInfo CPStageInfo::WellB("idoB", 0, 0, Direction::Right);
const CPStageInfo CPStageInfo::SchoolClassroom("school", 7, 4);
const CPStageInfo CPStageInfo::DevelopersLounge("admin", 10, 5, Direction::Up);
const CPStageInfo CPStageInfo::Seashore("seashore", 8, 4);
const CPStageInfo CPStageInfo::AdminsBar("admin_bar", 6, 9, Direction::Up);
const CPStageInfo CPStageInfo::RadioStudio("radio", 1, 9, Direction::Up);
const CPStageInfo CPStageInfo::RadioRoom1("radio_room1", 6, 1);
const CPStageInfo CPStageInfo::RamenStand("yatai", 7, 7, Direction::Up);
const CPStageInfo CPStageInfo::HilltopStairway("kaidan", 0, 10, Direction::Up);
const CPStageInfo CPStageInfo::Hilltop("takadai", 1, 13, Direction::Up);
const CPStageInfo CPStageInfo::DressingRoom("radio_gakuya", 8, 2);
const CPStageInfo CPStageInfo::Backstage("radio_backstage", 2, 6);
const CPStageInfo CPStageInfo::BeatGiko("radio_room2", 10, 4);
const CPStageInfo CPStageInfo::GSquid("radio_room3", 10, 8, Direction::Up);
const CPStageInfo CPStageInfo::SchoolHallway("school_rouka", 4, 3);
const CPStageInfo CPStageInfo::SchoolCulturalRoom("school_international", 8, 10);
const CPStageInfo CPStageInfo::SchoolComputerLab("school_pc", 0, 10, Direction::Right);
const CPStageInfo CPStageInfo::BanquetingHall("enkai", 20, 12);

const CPStageInfo CPStageInfo::Dogrun("dogrun", 0, 0);
const CPStageInfo CPStageInfo::Forest("forest", 0, 0);
const CPStageInfo CPStageInfo::Cafe("cafe", 0, 0);

const CPStageInfo CPStageInfo::Test1("test1", 0, 0);
const CPStageInfo CPStageInfo::Test2("test2", 0, 0);
const CPStageInfo CPStageInfo::Test3("test3", 0, 0);
const CPStageInfo CPStageInfo::Test4("test4", 0, 0);
const CPStageInfo CPStageInfo::Test5("test5", 0, 0);
const CPStageInfo CPStageInfo::Test6("test6", 0, 0);
const CPStageInfo CPStageInfo::Test7("test7", 0, 0);
const CPStageInfo CPStageInfo::Test8("test8", 0, 0);
const CPStageInfo CPStageInfo::Test9("test9", 0, 0);
const CPStageInfo CPStageInfo::Test10("test10", 0, 0);

const CPStageInfo *CPStageInfo::fromStageId(const QString &stageId)
{
    QByteArray tmp = stageId.toUtf8();
    StageIdHashEntry *ent = StageIdHash::lookup(tmp.constData(), tmp.length());

    if (ent != nullptr) {
        return ent->m_stageInfo;
    }

    return nullptr;
}
