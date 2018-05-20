#include "shared-object.hpp"

CPSharedObject::CPSharedObject(QObject *parent) :
    QObject(parent),
    m_name(),
    m_version(0)
{
}

void CPSharedObject::synchronize(const CPCodec &buf)
{
    // continue only if it is the same object
    QByteArray name;
    int i = 0;
    buf.decodeString(&i, &name);
    if (name != this->name().constData()) {
        return;
    }

    quint32 version;
    buf.decodeInt32(&i, &version);
    if (this->version() < version) {
        this->setVersion(version);
    }

    buf.decodeInt32(&i, &version); // flags
    buf.decodeInt32(&i, &version); // is persistent

    // now decode the events
    while (i < buf.size()) {
        switch (static_cast<CPSharedEventType>(buf.constData()[i++])) {
        case CPSharedEventType::UseSuccess:
            i += 4;
            if (buf.constData()[i] == static_cast<char>(CPSharedEventType::Clear)) {
                i += 5;
                emit this->connected();
            }
            break;
        case CPSharedEventType::Clear:
            i += 4;
            emit this->cleared();
            break;
        case CPSharedEventType::Change: { // this is the main one
                quint32 dataSize;
                QByteArray slotName;
                buf.decodeInt32(&i, &dataSize);
                int j = i;
                buf.decodeString(&j, &slotName);
                quint32 playerId = strtol(&slotName.constData()[1], NULL, 10);

                switch (slotName.constData()[0]) {
                case 'a': {
                        QByteArray playerName;
                        buf.decodeAmfString(&j, &playerName);
                        emit this->playerNameReceived(playerId, QString::fromUtf8(playerName));
                    }
                    break;
                case 'b': {
                        double cid;
                        buf.decodeAmfNumber(&j, &cid);
                        Character c = static_cast<Character>(int(cid));
                        emit this->playerCharacterIdReceived(playerId, c);
                    }
                    break;
                case 'd': {
                        QByteArray message;
                        buf.decodeAmfString(&j, &message);
                        emit this->playerMessageReceived(playerId, QString::fromUtf8(message));
                    }
                    break;
                case 'e': {
                        double dir;
                        buf.decodeAmfNumber(&j, &dir);
                        Direction d = static_cast<Direction>(int(dir));
                        emit this->playerDirectionChanged(playerId, d);
                    }
                    break;
                case 'f': {
                        QByteArray pos;
                        buf.decodeAmfString(&j, &pos);
                        int xpos = 0, ypos = 0;

                        QByteArrayList splits = pos.split(',');

                        if (splits.count() == 2) {
                            xpos = splits[0].toInt();
                            ypos = splits[1].toInt();

                            emit this->playerPositionChanged(playerId, xpos, ypos);
                        }
                    }
                    break;
                case 'g': {
                        double msgPos;
                        buf.decodeAmfNumber(&j, &msgPos);
                        MessagePosition mp = static_cast<MessagePosition>(int(msgPos));
                        emit this->playerMessagePositionChanged(playerId, mp);
                    }
                    break;
                }
                i += dataSize;
                break;
            } // changed
        case CPSharedEventType::SendMessage: {
                quint32 dataSize;
                buf.decodeInt32(&i, &dataSize);
                QByteArray function;
                buf.decodeAmfString(&i, &function);
                double playerId;
                buf.decodeAmfNumber(&i, &playerId);
                if (function == "manageRula") {
                    emit this->playerLeft(playerId, "Rula");
                } else if (function == "manageLogoutStage") {
                    emit this->playerLeft(playerId, "Stage Exit");
                } else if (function == "manageLogout") {
                    emit this->playerLeft(playerId, "Quit or Timedout");
                }
            }
            break;
        case CPSharedEventType::Remove: {
                // Remove Events only contain the slotName. Not data is sent.
                quint32 dataSize;
                buf.decodeInt32(&i, &dataSize);
                i += dataSize;
                // No need to remove as we don't keep track of slots :)
                // Wait for server to send the broadcast message.
            }
            break;
        default: {
                QString emsg = "SharedObject::synchronize: Could not decode event type '";
                emsg.append(QString::number(buf.constData()[i - 1]));
                emsg.append("'. Skipping.");
                emit this->error(emsg);
            }
        } // Event switch
    }
}

CPSharedEvent::CPSharedEvent(const CPSharedEvent &other) :
    CPAmfProperty(other),
    m_dataSize(other.m_dataSize),
    m_type(other.m_type)
{
}

CPSharedEvent::CPSharedEvent() :
    CPAmfProperty(),
    m_dataSize(0),
    m_type(CPSharedEventType::None)
{}

CPSharedEvent::CPSharedEvent(CPSharedEvent &&other) noexcept :
    CPAmfProperty(std::move(other)),
    m_dataSize(other.m_dataSize),
    m_type(other.m_type)
{
    other.m_dataSize = 0;
    other.m_type = CPSharedEventType::None;
}

CPSharedEvent &CPSharedEvent::operator=(const CPSharedEvent &other)
{
    this->m_dataSize = other.m_dataSize;
    this->m_type = other.m_type;
    this->m_name = other.m_name;
    this->m_data = other.m_data;

    return *this;
}

CPSharedEvent &CPSharedEvent::operator=(CPSharedEvent &&other) noexcept
{
    this->m_dataSize = other.m_dataSize;
    this->m_type = other.m_type;
    this->m_name = std::move(other.m_name);
    this->m_data = std::move(other.m_data);

    return *this;
}

void CPSharedEvent::setDataSize() noexcept
{
    quint32 size = CPCodec::stringSize(this->name());

    if (this->dataType() == CPAmfType::Number) {
        size += 9;
    } else if (this->dataType() == CPAmfType::String) {
        size += CPCodec::amfStringSize(*this->constString());
    } else if (this->dataType() == CPAmfType::Boolean) {
        size += CPCodec::amfBooleanSize();
    }

    this->m_dataSize = size;
}

CPSharedEvent::CPSharedEvent(CPSharedEventType type, const QByteArray &name) :
    CPAmfProperty(),
    m_type(type)
{
    this->setName(name);
    this->setDataSize();
}

CPSharedEvent::CPSharedEvent(CPSharedEventType type, const QByteArray &name,
                             const CPAmfVariant &data) :
    CPAmfProperty(name, data),
    m_type(type)
{
    this->setDataSize();
}

CPSharedEvent::~CPSharedEvent() noexcept
{
}
