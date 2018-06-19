#ifndef CUTIEPOI_NET_CONNECTION_HPP
#define CUTIEPOI_NET_CONNECTION_HPP

#include <QNetworkProxy>
#include <QByteArray>
#include <QSharedPointer>
#include <QTcpSocket>
#include <QVector>
#include <QDateTime>
#include <QTimer>
#include "channel.hpp"
#include "codec.hpp"
#include "shared-object.hpp"

struct CPRpc {
    QByteArray name;
    int transactionId;

    CPRpc();
    CPRpc(const QByteArray &name, int transactionId);
};

class CPConnection : public QObject {
    Q_OBJECT
public:
    enum Server : qint8 {
        General = 0,
        Foreign,
        Young,
        Vip,
        Kws,
    };
    Q_ENUM(Server)

    enum LimitType : quint8 {
        Hard,
        Soft,
        Dynamic
    };

    typedef void (CPConnection::*RpcHandler)(const QString &resp, int i);
    typedef QVector<CPChannel> ChunkCache;
private:

    struct BasicHeader {
        int channelId;
        CPHeaderType headerType;
    };

    QTcpSocket m_socket;
    QAtomicInteger<qint32> m_transactions;
    ChunkCache m_inputChannels;
    ChunkCache m_outputChannels;
    QVector<CPRpc> m_rpcs;
    CPChannel *m_activeChannel;
    CPCodec m_inputBuffer;
    QTime m_time;
    QTimer m_timer;
    QTimer m_timerForAvoidingTimeouts;
    quint32 m_inputChunkSize;
    quint32 m_outputChunkSize;

    quint32 m_inputWindowSize;
    quint32 m_lastInputSequence;
    quint32 m_bytesReceived;

    quint32 m_outputWindowSize;
    quint32 m_lastOutputSequence;
    quint32 m_bytesSent;

    LimitType m_bandwidthLimitType;
    Server m_server;
    CPSharedObject *m_sharedObject;

    static const RpcHandler RpcTable[];


public:
    CPConnection(QObject *parent = Q_NULLPTR);

    /**
     * Closes the connection.
     */
    Q_INVOKABLE void close();
public slots:
    /**
     * @brief conenctToHost Connect to the Gikopoi host.
     */
    void connectToHost();

    /**
     * @brief connectToServer Connect to the Application Server.
     *
     * @param[in] server The server to connect to. (gen | for | yng | vip | kws)
     * @param[in] username The user to login as.
     */
    void connectToServer(Server server, const QString &username);

    /**
     * Ignores a player.
     *
     * @param[in] playerId The player to ignore.
     */
    void ignorePlayer(int playerId);
    /**
     * Query the server's event list
     *
     * If dateTime is not provided, it defaults to the current minute.
     *
     * @param[in] dateTime The lower time limit to search for. Optional.
     * @param[in] name The event name to search for. Optional.
     * @param[in] stage The stage id to search for. Optional.
     * @param[in] rating The lower rating limit to search for. Optional.
     */
    void queryEventList(const QDateTime *dateTime = nullptr,
                        const QString *name = nullptr,
                        const QString *stage = nullptr,
                        int rating = 0);

    /**
     * Vote an event and increase its rating.
     *
     * @param[in] eventId the event to vote. Must identify an existing event.
     */
    void voteEvent(const QString &eventId);
    /**
     * Delete an event.
     *
     * @param[in] eventId The event to delete. Must identify an existing event.
     */
    void deleteEvent(const QString &eventId);
    /**
     * Register an event.
     *
     * @param[in] name The event name
     * @param[in] datetime The date and time at which the event takes place.
     * @param[in] stageId the stage
     * @param[in] description the event description
     * @param[in] imageUrl the url to the event image.
     * @param[in] website url to the event website.
     * @param[in] username the registering user.
     * @param[in] password the password for deleting the event.
     *
     * @return true if stageId is valid, false otherwise.
     */
    bool registerEvent(const QString &name,
                       const QDateTime &datetime,
                       const QString &stageId,
                       const QString &description,
                       const QString &imageUrl,
                       const QString &website,
                       const QString &username,
                       const QString &password);
    /**
     * Get the event details from the server.
     *
     * @param[in] eventId The event to get the details of.
     */
    void queryEventDetails(const QString &eventId);

    /**
     * Sends the current direction to the server.
     *
     * Use this method if a player changed the direction they are moving in.
     *
     * @param[in] xpos current position on the x-axis.
     * @param[in] ypos current position on the y-axis.
     * @param[in] direction The current direction.
     */
    void sendDirection(int xPos, int yPos, CPSharedObject::Direction direction);
    /**
     * Send the current position to the server.
     *
     * Use this method if a player is continuing to move in the same direction.
     *
     * Do not update the positions manually: let the server update it. Otherwise
     * your avatar may hang. Clients should wait for the server to update their
     * positions before calling this method.
     *
     * @param[in] xpos current position on the x-axis.
     * @param[in] ypos current position on the y-axis.
     * @param[in] direction The current direction.
     */
    void sendPosition(int xPos, int yPos, CPSharedObject::Direction direction);
    /**
     * Send the message position.
     * @param[in] position The new message position
     */
    void sendMessagePosition(CPSharedObject::MessagePosition position);

    void setPeerBandwidth(quint32 newWindowSize, LimitType limitType);
    /**
     * Set the input buffer.
     */
    inline void setInputBuffer(const QByteArray &buf);
    inline quint32 currentTime() const;

    inline quint32 outputChunkSize() const;

    inline void setProxy(const QNetworkProxy &networkProxy);
    inline void setProxy(const QString &hostName, const int port);

    inline CPChannel *inputChannel(int channel);
    inline CPChannel *outputChannel(int channel);

    void writeHeader(CPCodec &buffer,
                     int channel,
                     quint32 streamId,
                     CPMessageType mType,
                     quint32 messageSize,
                     quint32 timestamp);

    /**
     * Send the user password to the server.
     *
     * This method should be called with the appropriate parameter immediately when serverResponse
     * signal is raised with isResult set to true.
     *
     * @param[in] password The user's password. Pass an empty QByteArray if there is no password.
     */
    void sendPassword(const QString &password);
    /**
     * Send hash to server for verification.
     *
     * Must be called when loginDetailsRequested is emitted, and no additional login details are available.
     */
    void sendHash();
    /**
     * Send tripcode base to the server.
     *
     * Must be called when loginDetailsRequested is emitted.
     */
    void sendTripcodeBase(const QByteArray &tripBase);
    /**
     * Get the next input sequence number.
     */
    inline quint32 nextInputSequence();
    /**
     * Get the next output sequence number.
     */
    inline quint32 nextOutputSequence();
    /**
     * Check if an acknowledgement must be sent to the peer.
     */
    inline bool mustSendAcknowledgement();
    
    // Send empty message to prevent timeouts
    void preventTimeout();
private:
    /**
     * Parses the server response to requestStageLoginInfo().
     *
     * @param[in] res The server's response. Maybe _result or _error.
     * @param[in] i index to start parsing at.
     *
     * res is here because the server may send an _error response.
     */
    void onStageLoginInfo(const QByteArray &res, int i);
    void onAmf0SharedObject();
    void onAcknowledgement();
    void onAmf0Command();
    void onWindowAcknowledgementSize();
    void onSetPeerBandwidth();
    /**
     * Handler for UserControlMessage.
     */
    void onUserControlMessage();
    /**
     * Handler for StreamBegin UserControlEvent.
     */
    void onStreamBegin();
    void onSetChunkSize();
    /**
     * Called when an Abort Message was received.
     */
    void onAbortMessage();

    // NetConnection methods

    /**
     * Handles the Amf0Command 'manageClientId'.
     */
    void onManageClientId(const QByteArray &command, int index);
    /**
     * Handles the Amf0Command 'manageLoginCount'.
     */
    void onManageLoginCount(const QByteArray &command, int index);
    /**
     * Handles the Amf0Command 'manageLoginAfterPasswordCheck'
     */
    void onManageLoginAfterPasswordCheck(const QByteArray &command, int index);
    /**
     * Handles the Amf0Command 'manageTrip'
     */
    void onManageTrip(const QByteArray &command, int index);
    /**
     * Handles the Amf0Command 'manageIgnoreEach'
     *
     * emits the signal ignored();
     *
     * @param[in] command The command name. Unused.
     * @param[in] index The index to start at. Unused.
     */
    void onIgnored(const QByteArray &command, int index);
    /**
     * Handles the Amf0Commands 'manageSystemAlert', 'manageTimeMessage', and 'manageGeneralMessage'
     *
     * Emits the signal announcement.
     *
     * @param[in] command The command name
     * @param[in] index Unused.
     */
    void onAnnouncement(const QByteArray &command, int index);
    /**
     * Handles the Amf0Command 'manageEventList'.
     *
     * Emits the signal eventListReceived
     *
     * @param[in] command Unused.
     * @param[in] index Unused.
     */
    void onManageEventList(const QByteArray &command, int index);
    /**
     * Handles the Amf0Command 'manageVoteEvent'
     *
     * Emits the signal eventVoteStatusReceived.
     *
     * @param[in] command Unused.
     * @param[in] index Unused.
     */
    void onManageVoteEvent(const QByteArray &command, int index);
    /**
     * Handles the Amf0Command 'manageDeleteEvent'
     *
     * Emits the signal eventDeleted()
     *
     * @param[in] command Unused.
     * @param[in] index Unused.
     */
    void onManageDeleteEvent(const QByteArray &command, int index);
    /**
     * Handles the Amf0Command 'manageRegisterEvent'
     *
     * Emits the signal eventRegistered
     *
     * @param[in] command Unused.
     * @param[in] index Unused.
     */
    void onManageRegisterEvent(const QByteArray &command, int index);
    /**
     * Handles the Amf0Command 'manageEventDetails'
     *
     * Emits the signal eventDetailsReceived.
     *
     * @param[in] command Unused.
     * @param[in] index Unused.
     */
    void onManageEventDetails(const QByteArray &command, int index);

    int readChunk(CPCodec *buf);
    void sendAcknowledgment();
    void write(const CPCodec &buffer);
signals:
    /**
     * Emitted when the RTMP handshake has been completed.
     */
    void handshaken();
    void error(const QString &str);
    /**
     * Emitted when the server responds to NetConection.connect()
     *
     * @param[in] isResult Set to true if server responded with _result
     * @param[in] command The command name.
     * @param[in] prop I don't remember what this is.
     * @param[in] resp The body of the response.
     */
    void serverResponse(bool isResult, const QString &command);
    /**
     * Emitted when the server sends the client ID.
     */
    void clientIdReceived(quint32 clientId);
    /**
     * Emitted when the server sends the login count.
     */
    void loginCountChanged(quint32 loginCount);
    /**
     * Emitted when the server sends the character Id after checking the password.
     *
     * @param[in] charId the character Id for the password.
     */
    void characterIdReceived(quint32 charId);
    /**
     * Emitted when the server requests the login details. Either sendTripcodeBase or sendHash, but not both, should be called when this signal is emitted.
     */
    void loginDetailsRequested();
    /**
     * Emitted when the server sends a tripcode.
     */
    void tripcodeReceived(const QByteArray &tripcode);
    /**
     * This signal is emitted when the connection receives a shared object message. The shared objects
     * are connected to this signal when the Use event is sent.
     *
     * @param[in] buf contains the message data.
     */
    void sharedObjectChanged(const CPCodec &buf);
    /**
     * This signal is emitted when the server is waiting for the client to enter a stage.
     */
    void waitingForStageEntry();
    /**
     * This signal is emitted when the socket has been disconnected.
     */
    void disconnected();
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
     * Emitted whenever a player leaves a stage.
     *
     * @param[in] playerId The player that left.
     * @param[in] act How the player left.
     */
    void playerLeft(quint32 playerId, const QString &act);
    /**
     * Emitted when the server sends player character ID.
     *
     * @param[in] playerId The player who's character ID was sent.
     * @param[in] characterId The character ID for the player.
     */
    void playerCharacterIdReceived(quint32 playerId, CPSharedObject::Character characterId);
    /**
     * This signal is emitted when the server updates a player's message position.
     *
     * @param[in] playerId The player who's message position was updated.
     * @param[in] messagePosition The new message position.
     */
    void playerMessagePositionChanged(quint32 playerId,
                                      CPSharedObject::MessagePosition messagePosition);
    /**
     * This signal is emitted when the server updates a player's position.
     *
     * @param[in] playerId The player whos position was updated.
     * @param[in] xPos New position in the x axis.
     * @param[in] yPos New position in the y axis.
     */
    void playerPositionChanged(quint32 playerId, int xPos, int yPos);
    /**
     * This signal is emitted when the server updates a player's direction.
     *
     * @param[in] playerId The player who's direction was changed.
     * @param[in] direction The new Direction.
     */
    void playerDirectionChanged(quint32 playerId, CPSharedObject::Direction direction);
    /**
     * Emitted when a stage was successfuly entered.
     */
    void stageEntrySuccessful();
    /**
     * Emitted when stage login information was received.
     */
    void stageLoginInfoReceived(const QStringList &stages,
                                const QStringList &broadcasters,
                                const QVector<int> &loginCounts);
    /**
     * Emitted when another player ignores the current player.
     *
     * @param[in] ignoredBy The player that ignored the current player.
     */
    void ignored(int ignoredBy);
    /**
     * Emitted when an announcement message has been received.
     *
     * priority shows the message priority. A priority of 0 means that the message is a
     * general message, while a priority of 1 means that it is a time message. A priority
     * of 2 means that the message is a system alert.
     *
     * @param[in] priority The message priority
     * @param[in] message The message.
     */
    void announcement(int priority, const QString &message);
    /**
     * Emitted when the event list for the server has been received.
     *
     * QStringLists are uses here to make implementing this function easy. It
     * would probably be okay to implement this signal using a QVariantList of a
     * QObject derived class.
     *
     * Here, the properties of an event is split across the lists. So, the same
     * index in all lists holds the properties of a single event.
     *
     * @params[in] ids Contains event ids.
     * @params[in] names Contains event names.
     * @params[in] dates Contains event dates.
     * @params[in] times Contains event times.
     * @params[in] stages Contains event stages.
     * @params[in] users Contains the names of the users who registered the events.
     * @params[in] ratings Contains the event ratings.
     */
    void eventListReceived(const QStringList &ids,
                           const QStringList &names,
                           const QStringList &dates,
                           const QStringList &times,
                           const QStringList &stages,
                           const QStringList &users,
                           const QStringList &ratings);
    /**
     * Emitted when the server accepts or rejects an event vote.
     *
     * @params[in] accepted true if vote was accepted by the server.
     */
    void eventVoteStatusReceived(bool accepted);
    /**
     * Emitted when the server notifies after deleting an event.
     *
     * @param[in] deleted true if event was deleted.
     */
    void eventDeleted(bool deleted);

    /**
     * Emitted when the server notifies the event registration status.
     *
     * @param[in] registered true if event was registered.
     */
    void eventRegistered(bool registered);
    /**
     * Emitted when the server sends event details.
     */
    void eventDetailsReceived(const QString &name,
                              const QString &stageId,
                              const QDateTime &datetime,
                              const QString &imageUrl,
                              const QString &website,
                              const QString &username,
                              const QString &password);
public slots:
    void setWindowAcknowledgmentSize(quint32 newWindowSize);
    /**
     * Set the output chunk size for the next message and notify the peer.
     *
     * @param[in] newChunkSize The new chunk size.
     *
     * If newChunkSize is less than 0x1, it is set to 0x1, and if it is greater than 0xffffff, it
     * is set to 0xffffff;
     *
     */
    void setOutputChunkSize(quint32 newChunkSize);
    /**
     * Tell peer to abort to discard partially received message in a chunk stream.
     *
     * @param[in] streamId The Chunk Stream which the message to be discarded to belongs to.
     */
    void abortMessage(quint32 streamId);
    /**
     * Send an Amf0SharedObject message to the server.
     *
     * @param[in] so The shared object to operate on.
     * @param[in] events The events requested.
     */
    void sendSharedObjectRequest(CPSharedObject *so, const QVector<CPSharedEvent> &events);
    /**
     * Enter a stage.
     *
     * @param[in] stageId The stage to enter.
     * @param[in] charId Client's character.
     *
     */
    bool enterStage(const QString &stageId, CPSharedObject::Character charId);
    /**
     * Send a client message.
     *
     * @param[in] message The message to send.
     */
    void sendClientMessage(const QString &message);
    /**
     * Leave the current stage.
     *
     * @param[in] rula If true, use rula action to leave the stage.
     */
    void leaveStage(bool rula = true);

    /**
     * Request the server to send the stage login info.
     */
    void requestStageLoginInfo();
private slots:
    void sendC1();
    void receiveS1();
    void receiveS2();
    void processNextPacket();
    void onSocketError(QAbstractSocket::SocketError);
    void sendKeepAlive();
private:
    static CPChannel *channel(ChunkCache &cache, int channel);
};

inline quint32 CPConnection::currentTime() const
{
    return this->m_time.elapsed();
}

inline quint32 CPConnection::outputChunkSize() const
{
    return this->m_outputChunkSize;
}

inline void CPConnection::setProxy(const QNetworkProxy &networkProxy)
{
    this->m_socket.setProxy(networkProxy);
}

inline void CPConnection::setInputBuffer(const QByteArray &buf)
{
    this->m_inputBuffer = buf;
}

inline CPChannel *CPConnection::inputChannel(int channel)
{
    return this->channel(this->m_inputChannels, channel);
}

inline CPChannel *CPConnection::outputChannel(int channel)
{
    return this->channel(this->m_outputChannels, channel);
}

inline bool CPConnection::mustSendAcknowledgement()
{
    return (this->m_bytesReceived >= this->nextInputSequence());
}

inline quint32 CPConnection::nextInputSequence()
{
    return (this->m_lastInputSequence + this->m_inputWindowSize);
}

inline quint32 CPConnection::nextOutputSequence()
{
    return (this->m_lastOutputSequence + this->m_outputWindowSize);
}

void CPConnection::setProxy(const QString &hostName, const int port)
{
    QNetworkProxy proxy;

    proxy.setType(QNetworkProxy::Socks5Proxy);
    proxy.setHostName(hostName);
    proxy.setPort(port);

    this->setProxy(proxy);
}

#endif // CUTIEPOI_NET_CONNECTION_HXX
