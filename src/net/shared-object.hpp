#ifndef CUTIEPOI_NET_SHARED_OBJECT_HPP
#define CUTIEPOI_NET_SHARED_OBJECT_HPP

#include <QObject>
#include "amf.hpp"
#include "codec.hpp"

class CPSharedObject : public QObject {
    Q_OBJECT
private:
    QByteArray m_name;
    quint32 m_version;

public:
    enum Direction : quint8 {
        Left,
        Right,
        Up,
        Down
    };
    Q_ENUM(Direction);

    enum MessagePosition : quint8 {
        TopLeft,
        BottomRight,
        TopRight,
        BottomLeft
    };
    Q_ENUM(MessagePosition);

    enum Character : qint8 {
        None = -1,
        Giko = 1,
        Shii,
        Boon,
        SantaGiko,
        SantaShii,
        FuroshikiGiko,
        PianicaShii
    };
    Q_ENUM(Character)

    /**
     * Construct a SharedObject.
     * @param[in] parent The parent QObject.
     */
    CPSharedObject(QObject *parent = Q_NULLPTR);
    // setters
    /**
     * Set the shared object name.
     *
     * @param[in] name The new name.
     */
    inline void setName(const QByteArray &name);
    /**
     * Set the shared object version.
     *
     * @param[in] version The new version.
     */
    inline void setVersion(quint32 version) noexcept;
    // getters
    /**
     * @return the name of the shared object.
     */
    inline const QByteArray &name() const noexcept;
    /**
     * @return the current version of the Shared Object.
     */
    inline quint32 version() const noexcept;
public slots:
    /**
     * Synchronize the shared object.
     *
     * @param[in] buf The event buffer.
     */
    void synchronize(const CPCodec &buf);
signals:
    /**
     * This signal is emitted when the shared object is connected to the server.
     */
    void connected();
    /**
     * This signal is emitted when the server requests the shared object to be cleared.
     */
    void cleared();
    /**
     * This signal is emitted when the server sends a player name.
     *
     * @param[in] playerId the player id.
     * @param[in] playerName the playerName (includes tripcode, if present).
     */
    void playerNameReceived(quint32 playerId, const QString &playerName);
    /**
     * This signal is emitted when the server sends a player message.
     *
     * @param[in] playerId The player id.
     * @param[in] message The message.
     */
    void playerMessageReceived(quint32 playerId, const QString &message);
    /**
     * This signal is emitted when the server updates a player's position.
     *
     * @param[in] playerId The player whos position was updated.
     * @param[in] xpos New position in the x axis.
     * @param[in] ypos New position in the y axis.
     */
    void playerPositionChanged(quint32 playerId, int xpos, int ypos);
    /**
     * This signal is emitted when the server updates a player's direction.
     *
     * @param[in] playerId The player who's direction was changed.
     * @param[in] direction The new Direction.
     */
    void playerDirectionChanged(quint32 playerId, Direction direction);
    /**
     * This signal is emitted when the server updates a player's message position.
     *
     * @param[in] playerId The player who's message position was updated.
     * @param[in] messagePosition The new message position.
     */
    void playerMessagePositionChanged(quint32 playerId, MessagePosition messagePosition);
    /**
     * Emitted when the server sends player character ID.
     *
     * @param[in] playerId The player who's character ID was sent.
     * @param[in] character The character ID for the player.
     */
    void playerCharacterIdReceived(quint32 playerId, Character character);
    /**
     * Emitted whenever an error is encountered.
     */
    void error(const QString &emsg);
    /**
     * Emitted whenever a player leaves a stage.
     *
     * @param[in] playerId The player that left.
     * @param[in] act How the player left.
     */
    void playerLeft(quint32 playerId, const QString &act);
};

enum class CPSharedEventType : quint8 {
    None = 0,
    Use = 1,
    Release,
    RequestChange,
    Change,
    Success,
    SendMessage,
    Status,
    Clear,
    Remove,
    RequestRemove,
    UseSuccess
};

class CPSharedEvent : public CPAmfProperty {
private:
    quint32 m_dataSize;
    CPSharedEventType m_type;
public:
    /**
     * Construct a SharedEvent.
     */
    CPSharedEvent();
    /**
     * Copy construct a shared event.
     * @param[in] other The source event.
     */
    CPSharedEvent(const CPSharedEvent &other);
    /**
     * Move construct a SharedEvent.
     * @param[in,out] other The source event.
     */
    CPSharedEvent(CPSharedEvent &&other) noexcept;
    /**
     * Create a named shared event.
     * @param[in] type The event type.
     * @param[in] name The event name.
     */
    CPSharedEvent(CPSharedEventType type, const QByteArray &name);
    /**
     * Create a named shared event with data..
     * @param[in] type the event type.
     * @param[in] name the event name.
     * @param[in] data the event data.
     */
    CPSharedEvent(CPSharedEventType type, const QByteArray &name, const CPAmfVariant &data);
    /**
     * Destroy the SharedEvent.
     */
    virtual ~CPSharedEvent() noexcept;

    // getters
    /**
     * Get the event type.
     * @return the event type.
     */
    inline CPSharedEventType type() const noexcept;
    /**
     * Get the event data size.
     * @return the data size.
     */
    inline quint32 dataSize() const noexcept;
    /**
     * Get the data type.
     * @return the data type.
     */
    inline CPAmfType dataType() const;
    // setters
    /**
     * Calculate and set the dataSize for the event.
     */
    void setDataSize() noexcept;
    /**
     * Set the event type
     *
     * @param[in] type The new event type.
     */
    inline void setType(CPSharedEventType type) noexcept;
    /**
     * Copy assign a SharedEvent.
     * @param[in] other The SharedEvent to copy.
     */
    CPSharedEvent &operator=(const CPSharedEvent &other);
    /**
     * Move assign a SharedEvent
     * @param[in] other The SharedEvent to move.
     */
    CPSharedEvent &operator=(CPSharedEvent &&other) noexcept;
};

inline void CPSharedObject::setName(const QByteArray &name)
{
    this->m_name = name;
}

inline void CPSharedObject::setVersion(quint32 version) noexcept
{
    this->m_version = version;
}

inline const QByteArray &CPSharedObject::name() const noexcept
{
    return this->m_name;
}

inline quint32 CPSharedObject::version() const noexcept
{
    return this->m_version;
}

inline CPSharedEventType CPSharedEvent::type() const noexcept
{
    return this->m_type;
}

inline quint32 CPSharedEvent::dataSize() const noexcept
{
    return this->m_dataSize;
}

inline CPAmfType CPSharedEvent::dataType() const
{
    return this->CPAmfVariant::type();
}

inline void CPSharedEvent::setType(CPSharedEventType type) noexcept
{
    this->m_type = type;
}

#endif // CUTIEPOI_NET_SHARED_OBJECT_HPP
