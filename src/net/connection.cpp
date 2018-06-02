/**
 * This file contains code that is derived from the Red5 Media Server
 * (http://red5.org/), which is licensed under the Apache 2.0 license.
 *
 * A copy of a license is distributed with this project.
 */
#include "connection.hpp"
#include "stage-info.hpp"
#include "codec.hpp"
#include "user-control-event.hpp"
#include "rpc-hash.hpp"
#include <QThread>

static const char SERVER_KEY[] = "dt303eml8858792281";

static const char *serverNames[] = {
    "gen",
    "for",
    "yng",
    "vip",
    "kws"
};

const CPConnection::RpcHandler CPConnection::RpcTable[] = {
    reinterpret_cast<RpcHandler>(&CPConnection::onManageClientId),                // 0
    reinterpret_cast<RpcHandler>(&CPConnection::onManageLoginCount),              // 1
    reinterpret_cast<RpcHandler>(&CPConnection::onManageLoginAfterPasswordCheck), // 2
    reinterpret_cast<RpcHandler>(&CPConnection::onManageTrip),                    // 3
    reinterpret_cast<RpcHandler>(&CPConnection::waitingForStageEntry),            // 4
    reinterpret_cast<RpcHandler>(&CPConnection::onStageLoginInfo),                // 5
    reinterpret_cast<RpcHandler>(&CPConnection::onIgnored),                       // 6
    reinterpret_cast<RpcHandler>(&CPConnection::onAnnouncement),                  // 7
    reinterpret_cast<RpcHandler>(&CPConnection::onManageEventList),               // 8
    reinterpret_cast<RpcHandler>(&CPConnection::onManageVoteEvent),               // 9
    reinterpret_cast<RpcHandler>(&CPConnection::onManageDeleteEvent),             // 10
    reinterpret_cast<RpcHandler>(&CPConnection::onManageRegisterEvent),           // 11
    reinterpret_cast<RpcHandler>(&CPConnection::onManageEventDetails),            // 12
};

void CPConnection::connectToHost()
{
    fprintf(stderr, "before m_socket.connectToHost\n");
    this->m_socket.connectToHost("tswindy.serveftp.net", 1937);
    fprintf(stderr, "after m_socket.connectToHost\n");
    this->m_socket.waitForConnected();
    fprintf(stderr, "after m_socket.connectToHost\n");
}

void CPConnection::sendC1()
{
    fprintf(stderr,"sendC1\n");
    CPCodec c1;
    c1.append(static_cast<char>(3));
    this->m_time.start();
    c1.appendInt32(this->currentTime());
    c1.resize(1537);
    memset(&c1.data()[5], 0,  1532);

    QObject::connect(&this->m_socket, &QTcpSocket::readyRead, this, &CPConnection::receiveS1);
    this->m_socket.write(c1);

}

void CPConnection::receiveS1()
{
    fprintf(stderr,"receiveS1\n");
    // Wait until enough data is available
    if (this->m_socket.bytesAvailable() < 1537) {
        return;
    }

    QObject::disconnect(&this->m_socket, &QTcpSocket::readyRead, this, &CPConnection::receiveS1);

    CPCodec s1;
    s1.resize(1537);
    char *data = s1.data();

    this->m_socket.read(data, 1537);

    if (data[0] != 3) {
        emit this->error("Server is using an unsupported RTMP version.");
        this->close();
        return;
    }

    quint32 serverZeroes;
    quint32 serverTime;

    int i = 1;
    s1.decodeInt32(&i, &serverTime);
    s1.decodeInt32(&i, &serverZeroes);

    if (serverZeroes != 0) {
        // emit this->error("S1.zeroes != 0");
        // this->close();
        // return;
    }

    // Send C2
    CPCodec c2;
    c2.appendInt32(serverTime);
    c2.appendInt32(this->currentTime());
    c2.append(&data[9], s1.size() - 9);
    QObject::connect(&this->m_socket, &QTcpSocket::readyRead, this, &CPConnection::receiveS2);
    this->m_socket.write(c2);
}

void CPConnection::receiveS2()
{
    fprintf(stderr,"receiveS2\n");
    // Wait until enough bytes are available
    if (this->m_socket.bytesAvailable() < 1536) {
        return;
    }

    QObject::disconnect(&this->m_socket, &QTcpSocket::readyRead, this, &CPConnection::receiveS2);

    // sendS2
    CPCodec s2;
    s2.resize(1536);
    this->m_socket.read(s2.data(), 1536);

    quint32 ct;
    int i = 0;
    s2.decodeInt32(&i, &ct);

#if 0
    // This no longer would work :)
    if (ct != this->currentTime()) {
        emit this->error("S2.peerTime != C1.time");
        this->close();
        return;
    }
#endif

    // Emit a signal here so that the user can call this->connectToServer()
    // with the appropriate parameters.
    fprintf(stderr,"about to emit this->handshaken();\n");
    emit this->handshaken();
}

void CPConnection::connectToServer(CPConnection::Server server,
                                   const QString &username)
{
    QByteArray app = "gikopoi141_";
    app.append(serverNames[server], 3);

    CPAmfObject obj;
    {
        obj.append(CPAmfProperty(QByteArray("app"), app));
    }

    {
        obj.append(CPAmfProperty("flashVer", "CutiePoi 0.1.5"));
    }

    {
        QByteArray swfUrl = "http://l4cs.jpn.org/gikopoi/flash/";
        swfUrl.append(app);
        swfUrl.append("/gikopoi.swf");
        obj.append(CPAmfProperty("swfUrl", swfUrl));
    }

    {
        QByteArray tcUrl = "rtmp://tswindy.serveftp.net:1937/";
        tcUrl.append(app);
        obj.append(CPAmfProperty("tcUrl", tcUrl));
    }

    bool isUsingProxy = this->m_socket.proxy() != QNetworkProxy::NoProxy;

    obj.append(CPAmfProperty("fpad", isUsingProxy));
    obj.append(CPAmfProperty("capabilities", double(239)));
    obj.append(CPAmfProperty("audioCodecs", double(0)));
    obj.append(CPAmfProperty("videoCodecs", double(0)));
    obj.append(CPAmfProperty("videoFunction", double(0)));

    {
        QByteArray pageUrl = "http://l4cs.jpn.org/gikopoi/flash/";
        pageUrl.append(app);
        pageUrl.append("/flash_gikopoi.html");
        obj.append(CPAmfProperty("pageUrl", pageUrl));
    }

    quint32 size = CPCodec::amfObjectSize(obj);
    QByteArray uname = username.toUtf8();
    size += CPCodec::amfStringSize(uname);
    size += CPCodec::amfStringSize(SERVER_KEY);
    size += CPCodec::amfStringSize("connect");
    size += CPCodec::amfNumberSize();

    CPCodec buf;

    buf.setChannel(3);
    buf.appendChunkHeader(CPHeaderType::Complete);
    buf.appendInt24(0);   // timestamp
    buf.appendInt24(size); // messageSize
    buf.append(static_cast<char>(CPMessageType::Amf0Command));
    buf.appendInt32(0);   // streamId

    buf.setChannel(3);
    buf.setChunkSize(128);
    buf.setHeaderSize(12);

    buf.appendAmfString("connect");
    buf.appendAmfNumber(1);

    this->m_transactions.fetchAndStoreOrdered(2);

    buf.appendAmfObject(obj);
    buf.appendAmfString(uname);
    buf.appendAmfString(SERVER_KEY);

    CPChannel *curChannel = this->outputChannel(3);

    curChannel->setMessageSize(size);
    curChannel->setMessageType(CPMessageType::Amf0Command);
    curChannel->setStreamId(0);
    curChannel->setTimestamp(1);

    this->m_server = server;
    this->m_socket.write(buf.constData(), buf.size());
    this->m_rpcs.append(CPRpc("connect", 1));
    QObject::connect(&this->m_socket, &QTcpSocket::readyRead, this, &CPConnection::processNextPacket);
}

void CPConnection::processNextPacket()
{
    // Read input and send acknowledgement if required.
    {
        const QByteArray &bytes = this->m_socket.readAll();
        this->m_bytesReceived += bytes.size();
        if (this->mustSendAcknowledgement()) {
            this->sendAcknowledgment();
            this->m_lastInputSequence += this->m_inputWindowSize;
        }

        this->m_inputBuffer.append(bytes);
    }
    // TODO: find the actual minimum size of an RTMP packet
    if (this->m_inputBuffer.size() < 3) {
        return;
    }

    do {
        if (this->readChunk(&this->m_inputBuffer) != 0) {
            return;
        }

        // Check for remaining chunks
        if (this->m_activeChannel->bytesPending() != 0 && this->m_inputBuffer.size() > 0) {
            this->m_activeChannel = Q_NULLPTR;
            continue;
        }

        CPMessageType mType = this->m_activeChannel->messageType();
        switch (mType) {
        case CPMessageType::UserControlMessage:
            this->onUserControlMessage();
            break;
        case CPMessageType::Amf0Command:
            this->onAmf0Command();
            break;
        case CPMessageType::WindowAcknowledgementSize:
            this->onWindowAcknowledgementSize();
            break;
        case CPMessageType::Aknowledgement:
            this->onAcknowledgement();
            break;
        case CPMessageType::PeerBandwidth:
            this->onSetPeerBandwidth();
            break;
        case CPMessageType::ChunkSize:
            this->onSetChunkSize();
            break;
        case CPMessageType::Amf0SharedObject:
            this->onAmf0SharedObject();
            break;
        default: {
                QString emsg("Unsupported command type: ");
                emsg.append(QString::number(static_cast<int>(mType), 10));
                emit this->error(emsg);
            }
            break;
        }

        this->m_activeChannel->done();
        this->m_activeChannel = Q_NULLPTR;
    } while (this->m_inputBuffer.size() > 3);
}

void CPConnection::onAmf0SharedObject()
{
    quint32 size = this->m_activeChannel->messageSize();

    // This is the minimum size of a shared object
    if (size < 19) {
        emit this->error("Corrupted SO packet received. channel->messageSize()");
        return;
    }

    QByteArray objectName;
    int pos = 0;
    CPCodec &buf = this->m_activeChannel->buffer();
    if (buf.decodeString(&pos, &objectName) != CPCodec::Status::Ok) {
        // emit an error?
        return;
    }

    if (objectName.size() == 0) {
        // emit an erro?
    }

    quint32 version;
    if (buf.decodeInt32(&pos, &version) != CPCodec::Status::Ok) {
        //
        return;
    }

    // bool isPersistent;
    {
        quint32 val;
        buf.decodeInt32(&pos, &val);
        // isPersistent = (val == 2);
    }

    quint32 reserved;
    buf.decodeInt32(&pos, &reserved);

    int i;
    for (i = 0; i < this->m_inputChannels.size(); ++i) {
        CPChannel *c = &this->m_inputChannels[i];
        if (c == this->m_activeChannel) {
            break;
        }
    }

    // It may be better to call so->synchronize, here.
    emit this->sharedObjectChanged(buf);
}

void CPConnection::onAmf0Command()
{
    // what is the call structure like?
    QByteArray command;
    CPChannel *activeChannel = this->m_activeChannel;
    const CPCodec &buf = activeChannel->constBuffer();
    double txId;

    int i = 0;

    buf.decodeAmfString(&i, &command);
    buf.decodeAmfNumber(&i, &txId);

    if (txId == 0) {
        RpcHashEntry *ent = RpcHash::lookup(command.constData(), command.size());
        if (ent) {
            (this->*CPConnection::RpcTable[ent->m_offset])(command, i);
        } else  {
            QString emsg("Unknown Amf0Command '");
            emsg.append(command);
            emsg.append("' received.");
            emit this->error(emsg);
        }
        return;
    }

    // _result or _error for Connect
    if (txId == 1) {
        CPAmfObject resp;
        CPAmfObject prop;

        // Would probably always be NULL.
        if (buf.constData()[i] == static_cast<char>(CPAmfType::Null)) {
            prop.append(CPAmfProperty());
            ++i;
        } else if (buf.decodeAmfObject(&i, &prop) != CPCodec::Status::Ok) {
            emit this->error("Couldn't decode server response");
            return;
        }

        if (buf.decodeAmfObject(&i, &resp) != CPCodec::Status::Ok) {
            emit this->error("Couldn't decode server response");
            return;
        }

        // TODO: send resp and prop to the application
        bool isResult = (command == "_result");
        emit this->serverResponse(isResult, QString::fromUtf8(command));

        if (isResult) {
            this->m_timer.start(59900);
        } else {
            this->close();
        }
    }

    // Remove the rpc from the queue
    CPRpc *rpc = Q_NULLPTR;
    int j;
    for (j = 0; j < m_rpcs.count(); ++j) {
        if (this->m_rpcs[j].transactionId == int(txId)) {
            rpc = &this->m_rpcs[j];
            break;
        }
    }

    if (rpc == Q_NULLPTR) {
        QString emsg = "Received response for transaction '";
        emsg.append(QString::number(int(txId), 10));
        emsg.append("' (");
        emsg.append(command);
        emsg.append("), but no RPC was registered");
        emit this->error(emsg);
        return;
    } else {
        if (rpc->name == "manageStageLoginCount") {
            this->onStageLoginInfo(command, i);
        }
        this->m_rpcs.remove(j);
    }
}

CPRpc::CPRpc(const QByteArray &name, int transactionId)
    : name(name), transactionId(transactionId)
{
}

CPRpc::CPRpc()
    : name(), transactionId(0)
{
}

CPConnection::CPConnection(QObject *parent) :
    QObject(parent),
    m_socket(),
    m_transactions(0),
    m_inputChannels(3),
    m_outputChannels(3),
    m_rpcs(1),
    m_activeChannel(Q_NULLPTR),
    m_inputBuffer(),
    m_time(),
    m_timer(),

    m_inputChunkSize(128),
    m_outputChunkSize(128),

    m_inputWindowSize(10000000),
    m_lastInputSequence(0),
    m_bytesReceived(0),

    m_outputWindowSize(10000000),
    m_lastOutputSequence(0),
    m_bytesSent(0),
    m_bandwidthLimitType(LimitType::Hard),
    m_sharedObject(nullptr)
{
    this->m_socket.setProxy(QNetworkProxy::NoProxy);

    QObject::connect(&this->m_socket, &QAbstractSocket::disconnected, this,
                     &CPConnection::disconnected);
    QObject::connect(&this->m_socket, &QTcpSocket::connected, this, &CPConnection::sendC1);

    typedef void (QAbstractSocket::*socketErrorMethod)(QAbstractSocket::SocketError);
    QObject::connect(&this->m_socket, static_cast<socketErrorMethod>(&QAbstractSocket::error),
                     this, &CPConnection::onSocketError);
    QObject::connect(&this->m_timer, &QTimer::timeout, this, &CPConnection::sendKeepAlive);
}

int CPConnection::readChunk(CPCodec *buf)
{
    CPChannel tmpChan;
    const char *cd = buf->constData();
    CPChannel *channel;
    int i = 0;
    CPHeaderType hType;

    {
        quint8 cid = cd[0] & 0x3f;
        hType = static_cast<CPHeaderType>(cd[0] & 0xc0);

        if (cid == 0) {
            if (buf->size() < 2) {
                return -1;
            }
            cid = cd[1] + 64;
            i = 2;
        } else if (cid == 1) {
            if (buf->size() < 3) {
                return -1;
            }
            cid = ((cd[2] << 8)  | cd[1]) + 64;
            i = 3;
        } else {
            i = 1;
        }

        channel = this->inputChannel(cid);
        tmpChan = *channel;
    }

    // size check
    if (buf->size() < i + 11 && hType == CPHeaderType::Complete) {
        return -1;
    } else if (buf->size() < i + 7 && hType == CPHeaderType::UpdateData) {
        return -1;
    } else if (buf->size() < i + 3 && hType == CPHeaderType::UpdateTimestamp) {
        return -1;
    }

    bool hasExtendedTimestamp = channel->hasExtendedTimestamp();
    if (hType <= CPHeaderType::UpdateTimestamp) {
        quint32 t;
        buf->decodeInt24(&i, &t);

        if (hType == CPHeaderType::Complete) {
            tmpChan.setTimestamp(t);
        } else {
            tmpChan.setTimestampDelta(t);
        }

        hasExtendedTimestamp = (t == 0xffffff);
        tmpChan.setHasExtendedTimestamp(hasExtendedTimestamp);
    }

    // Get messageSize
    if (hType <= CPHeaderType::UpdateData) {
        quint32 messageSize;
        buf->decodeInt24(&i, &messageSize);
        tmpChan.setMessageSize(messageSize);
        tmpChan.setMessageType(static_cast<CPMessageType>(cd[i]));
        ++i;
    }

    if (hType == CPHeaderType::Complete) {
        quint32 streamId;
        buf->decodeInt32Le(&i, &streamId);
        tmpChan.setStreamId(streamId);
    }

    if (hasExtendedTimestamp) {
        if (buf->size() < i + 4) {
            return -1;
        }

        quint32 extendedTimestamp;
        buf->decodeInt32(&i, &extendedTimestamp);
        tmpChan.setExtendedTimestamp(extendedTimestamp);
    }

    // need to determine the number of chunks the message will be split into
    // but would have to read it chunk by chunk as a message in another channel
    // may be next to this. Also, this removes the message headers from the
    // buffers, making some commands constant sized. Also makes handling stuff
    // easier: data always starts at the 0th offset.
    quint32 bytesToRead;
    if (tmpChan.bytesPending() >= this->m_inputChunkSize) {
        // more bytes remaining than chunkSize: read a chunk
        bytesToRead = this->m_inputChunkSize;
    } else {
        // less bytes than chunkSize: read the whole thing
        bytesToRead = tmpChan.bytesPending();
    }

    // not enough data in the input buffer
    if (bytesToRead > (quint32)(buf->size() - i)) {
        return -1;
    }

    tmpChan.buffer().append(&cd[i], bytesToRead);
    buf->remove(0, i + bytesToRead);
    this->m_activeChannel = channel;
    *channel = tmpChan;
    return 0;
}

void CPConnection::onWindowAcknowledgementSize()
{
    if (this->m_activeChannel->messageSize() != 4) {
        emit this->error("Corrupted WindowAcknowledgementSize message received.");
        return;
    }

    quint32 newWindowSize;
    int i = 0;


    if (this->m_activeChannel->buffer().decodeInt32(&i, &newWindowSize) == CPCodec::Status::Ok) {
        this->m_inputWindowSize = newWindowSize;
    }
}

void CPConnection::sendAcknowledgment()
{
    CPCodec buf;
    this->writeHeader(buf, 2, 0, CPMessageType::Aknowledgement, 4, this->currentTime());
    buf.appendInt32(this->nextInputSequence());
    this->write(buf);
}

void CPConnection::writeHeader(CPCodec &buffer,
                               int channel,
                               quint32 streamId,
                               CPMessageType mType,
                               quint32 messageSize,
                               quint32 timestamp)
{
    CPChannel *c = this->outputChannel(channel);
    CPHeaderType hType = CPHeaderType::Complete;
    quint32 ts = timestamp;
    quint32 extTimestamp = ts;

    if (ts > 0xffffff) {
        ts = 0xffffff;
    }


    // check if a type 1 packet can be sent
    if (c->streamId() == streamId) {
        hType = CPHeaderType::UpdateData;

        // both type 1 and 2 use timestampe deltas. Calculate the delta.
        ts = c->timestamp() - timestamp;
        extTimestamp = ts;
        if (ts >= 0xffffff) {
            ts = 0xffffff;
        }

        // check if a type 2 packet can be sent
        if (c->messageType() == mType && c->messageSize() == messageSize) {
            hType = CPHeaderType::UpdateTimestamp;

            if (ts == c->timestampDelta()) {
                if (ts != 0xffffff || extTimestamp == c->extendedTimeValue()) {
                    hType = CPHeaderType::Minimal;
                }
            }
        }
    }

    buffer.setChannel(channel);
    buffer.appendChunkHeader(hType);
    int bytes = buffer.size();

    if (hType <= CPHeaderType::UpdateTimestamp) {
        if (hType == CPHeaderType::Complete) {
            c->setTimestamp(ts);
        } else {
            c->setTimestampDelta(ts);
        }
        buffer.appendInt24(ts);
        bytes += 3;
    }

    if (hType <= CPHeaderType::UpdateData) {
        buffer.appendInt24(messageSize);
        buffer.append(static_cast<char>(mType));

        c->setMessageSize(messageSize);
        c->setMessageType(mType);
        bytes += 4;
    }

    if (hType == CPHeaderType::Complete) {
        c->setStreamId(streamId);
        buffer.appendInt32Le(streamId);
        bytes += 4;
    }

    if (ts == 0xffffff) {
        c->setExtendedTimestamp(extTimestamp);
        buffer.appendInt32(extTimestamp);
        buffer.setExtendedTimeValue(extTimestamp);
        buffer.setHasExtendedTimeValue(true);
        bytes += 4;
    }

    buffer.setHeaderSize(bytes);
    buffer.setChannel(channel);
}

void CPConnection::onSetPeerBandwidth()
{
    if (this->m_activeChannel->messageSize() != 5) {
        emit this->error("Corrupted SetPeerBandwidth message received.");
        return;
    }

    int i = 0;
    quint32 bw;
    this->m_activeChannel->buffer().decodeInt32(&i, &bw);
    i = this->m_activeChannel->buffer().constData()[i];

    if (i == 1 && this->m_outputWindowSize <= bw) {
        return;
    }

    if (i == 2 && this->m_bandwidthLimitType != LimitType::Hard) {
        return;
    }

    if (i == 2) {
        i = 0;
    }

    this->m_bandwidthLimitType = static_cast<LimitType>(i);
    this->setWindowAcknowledgmentSize(bw);
}

void CPConnection::setWindowAcknowledgmentSize(quint32 newWindowSize)
{
    CPCodec buffer;

    this->writeHeader(buffer,
                      2,
                      0,
                      CPMessageType::WindowAcknowledgementSize,
                      4,
                      this->currentTime());

    buffer.appendInt32(newWindowSize);
    this->m_outputWindowSize = newWindowSize;
    this->write(buffer);
}

void CPConnection::setOutputChunkSize(quint32 newChunkSize)
{
    CPCodec buffer;
    this->writeHeader(buffer, 2, 0, CPMessageType::ChunkSize, 4, this->currentTime());

    if (newChunkSize == 0) {
        newChunkSize = 1;
    } else if (newChunkSize > 0xffffff) {
        newChunkSize = 0xffffff;
    }

    buffer.appendInt32(newChunkSize);
    this->m_outputChunkSize = newChunkSize;

#if 0
    QVector<CPChannel>::iterator i;
    for (i = this->m_outputChannels.begin(); i != this->m_outputChannels.end(); ++i) {
        i->setChunkSize(chunkSize);
    }
#endif

    this->write(buffer);
}

void CPConnection::onSetChunkSize()
{
    if (this->m_activeChannel->messageSize() != 4) {
        emit this->error("Corrupted Set Peer Chunk Size message: Invalid size.");
        return;
    }

    quint32 chunkSize;
    int i;
    this->m_activeChannel->buffer().decodeInt32(&i, &chunkSize);
    if (!!(chunkSize & (1 << 31))) {
        emit this->error("Corrupted Set Peer Chunk Size message: Invalid payload.");
        return;
    }

    this->m_inputChunkSize = chunkSize;

#if 0
    QVector<CPChannel>::iterator i;
    for (i = this->m_inputChannels.begin(); i != this->m_inputChannels.end(); ++i) {
        i->setChunkSize(chunkSize);
    }
#endif
}

void CPConnection::onAbortMessage()
{
    if (this->m_activeChannel->messageSize() != 4) {
        emit this->error("Corrupted Abort Message Packet");
        return;
    }

    int i = 0;
    quint32 csid;
    this->m_activeChannel->buffer().decodeInt32(&i, &csid);

    CPChannel *c = this->inputChannel(csid);

    if (c) {
        c->reset();
    } else {
        emit this->error("Corrupted Abort Message: Invalid Chunk Stream Id");
    }
}

void CPConnection::abortMessage(quint32 csid)
{
    CPCodec buf;

    this->writeHeader(buf, 2, 0, CPMessageType::Abort, 4, this->currentTime());
    buf.appendInt32(csid);
    this->write(buf);
}

void CPConnection::onAcknowledgement()
{
    if (this->m_activeChannel->messageSize() != 4) {
        emit this->error("Corrupted Acknowledgement");
        return;
    }

    int i = 0;
    this->m_activeChannel->buffer().decodeInt32(&i, &this->m_lastOutputSequence);
}

void CPConnection::write(const CPCodec &buffer)
{
    this->m_bytesSent += buffer.size();
    this->m_socket.write(buffer.constData(), buffer.size());
}

void CPConnection::setPeerBandwidth(quint32 newWindowSize, LimitType limitType)
{
    CPCodec buf;
    this->writeHeader(buf, 2, 0, CPMessageType::PeerBandwidth, 5, this->currentTime());

    buf.appendInt32(newWindowSize);
    buf.append(static_cast<char>(limitType));
    this->write(buf);
}

void CPConnection::onUserControlMessage()
{
    if (this->m_activeChannel->messageSize() < 4) {
        emit this->error("Corrupted UserControlMessage");
        return;
    }

    quint16 eventType;
    int i = 0;
    this->m_activeChannel->buffer().decodeInt16(&i, &eventType);

    switch (eventType) {
    case StreamBegin:
        this->onStreamBegin();
        break;
    default: {
            QString emsg("Unsupported UserControlEvent: ");
            emsg.append(QString::number(eventType, 10));
            emit this->error(emsg);
            break;
        }
    }
}

void CPConnection::onStreamBegin()
{
    int i = 2;
    quint32 streamId;
    this->m_activeChannel->buffer().decodeInt32(&i, &streamId);

    if (streamId == 0) {
        return;
    } else {
        // TODO: Support playing streams
    }
}

void CPConnection::onManageClientId(const QByteArray &command, int i)
{
    const CPCodec &buf = this->m_activeChannel->buffer();

    // This is fixed sized
    if (buf.size() != 36) {
        emit this->error("Received corrupted manageClientId command");
        return;
    }

    i = 27;
    double clientId;

    if (buf.decodeAmfNumber(&i, &clientId) != CPCodec::Status::Ok) {
        emit this->error("Failed to decode manageClientId command");
        return;
    }

    emit this->clientIdReceived(clientId);
}

void CPConnection::onManageLoginCount(const QByteArray &command, int i)
{
    Q_UNUSED(command);
    const CPCodec &buf = this->m_activeChannel->buffer();

    // This is a fixed sized message
    if (buf.size() != 38) {
        emit this->error("Corrupted manageLoginCount command received.");
        return;
    }

    i = 29;
    double loginCount;

    if (buf.decodeAmfNumber(&i, &loginCount) != CPCodec::Status::Ok) {
        emit this->error("Failed to decode manageLoginCount command");
        return;
    }

    emit this->loginCountChanged(loginCount);
}

void CPConnection::sendPassword(const QString &password)
{
    // can't send data over a closed socket
    if (!this->m_socket.isOpen()) {
        return;
    }

    CPCodec buf;
    QByteArray passwd = password.toUtf8();
    this->writeHeader(buf, 3, 0, CPMessageType::Amf0Command, 27 + CPCodec::amfStringSize(passwd),
                      this->currentTime());
    buf.appendAmfString("managePassword");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfString(passwd);

    this->write(buf);
}

void CPConnection::sendHash()
{
    CPCodec buf;

    // Maybe time should be updated?
    this->writeHeader(buf, 3, 0, CPMessageType::Amf0Command, 73, this->currentTime());
    buf.appendAmfString("manageCheckHash");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));

    switch (this->m_server) {
    case General:
        buf.appendAmfNumber(7970189);
        buf.appendAmfNumber(6119501);
        buf.appendAmfNumber(6390689);
        buf.appendAmfNumber(4323731);
        break;
    case Foreign:
        buf.appendAmfNumber(4111106);
        buf.appendAmfNumber(223160);
        buf.appendAmfNumber(282641);
        buf.appendAmfNumber(180392);
        break;
    case Young:
        buf.appendAmfNumber(189887);
        buf.appendAmfNumber(6119501);
        buf.appendAmfNumber(6390689);
        buf.appendAmfNumber(1601483);
        break;
    case Vip:
        buf.appendAmfNumber(371336);
        buf.appendAmfNumber(1739183);
        buf.appendAmfNumber(6390689);
        buf.appendAmfNumber(2693);
        break;
    case Kws:
        buf.appendAmfNumber(346550);
        buf.appendAmfNumber(6119501);
        buf.appendAmfNumber(6390689);
        buf.appendAmfNumber(193973);
        break;
    }

    buf.appendAmfNumber(1822258);

    this->write(buf);
}

void CPConnection::onManageLoginAfterPasswordCheck(const QByteArray &command, int i)
{
    const CPCodec &buf = this->m_activeChannel->buffer();

    i = 42;
    double serverTime;
    buf.decodeAmfNumber(&i, &serverTime);

    // Not sure if the server sends an AmfNumber or an AmfString when the
    // password check was successful. Check for both. The server sends an
    // empty string if the password check fails or if an empty password
    // was sent.
    if (buf.constData()[i] == static_cast<char>(CPAmfType::String)) {
        QByteArray charIdStr;
        buf.decodeAmfString(&i, &charIdStr);

        if (charIdStr.size() != 0) {
            bool ok;
            quint32 charId = charIdStr.toInt(&ok, 10);
            if (ok) {
                emit this->characterIdReceived(charId);
            } else {
                // try hex too
                charId = charIdStr.toInt(&ok, 16);
                if (ok) {
                    emit this->characterIdReceived(charId);
                } else {
                    // Should probably dump the packet here
                    emit this->error("Could not convert character Id to an int");
                }
            }
        }
    } else {
        // Assume that it is sent as an AmfNumber
        double charId;
        if (buf.decodeAmfNumber(&i, &charId) != CPCodec::Status::Ok) {
            emit this->error("Could not decode command onManageLoginAfterPasswordCheck");
        }

        emit this->characterIdReceived(charId);
    }

    // The rest of the parameters aren't needed
    emit this->loginDetailsRequested();
}

void CPConnection::onManageTrip(const QByteArray &command, int i)
{
    const CPCodec &buf = this->m_activeChannel->buffer();

    if (buf.size() != 36) {
        emit this->error("Corrupted onManageTrip command received.");
        return;
    }

    i = 23;
    QByteArray trip;
    if (buf.decodeAmfString(&i, &trip) != CPCodec::Status::Ok) {
        emit this->error("Could not decode tripcode.");
        return;
    }

    emit this->tripcodeReceived(trip);
}

void CPConnection::sendTripcodeBase(const QByteArray &tripBase)
{
    CPCodec buf;

    this->writeHeader(buf, 3, 0, CPMessageType::Amf0Command, 23 + CPCodec::amfStringSize(tripBase),
                      this->currentTime());
    buf.appendAmfString("manageTrip");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfString(tripBase);

    this->write(buf);
    this->sendHash();
}

void CPConnection::sendSharedObjectRequest(CPSharedObject *so, const QVector<CPSharedEvent> &events)
{
    CPCodec buf;

    quint32 messageSize = CPCodec::stringSize(so->name());
    // the shared object meta info is 12 bytes, excluding the name
    messageSize += 12;

    foreach (const CPSharedEvent &event, events) {
        messageSize += event.dataSize() + 5;
    }

    this->writeHeader(buf, 3, 0, CPMessageType::Amf0SharedObject, messageSize,
                      currentTime());

    buf.appendString(so->name());
    buf.appendInt32(so->version());
    // the following are always set to zero
    buf.appendInt32(0);
    buf.appendInt32(0);

    // Now lets get to the events
    foreach (const CPSharedEvent &event, events) {
        buf.append(static_cast<char>(event.type()));
        buf.appendInt32(event.dataSize());

        // Skip writing data if there isn't any
        if (event.dataSize() == 0) {
            continue;
        } else {
            buf.appendString(event.constName());
        }

        // skip the data if only the name is to be written
        if (event.dataSize() == (quint32)CPCodec::stringSize(event.constName())) {
            continue;
        }

        switch (event.dataType()) {
        case CPAmfType::Number:
            buf.appendAmfNumber(event.number());
            break;
        case CPAmfType::String:
            buf.appendAmfString(*event.constString());
            break;
        case CPAmfType::Boolean:
            buf.appendAmfBoolean(event.boolean());
            break;
        default:
            QString emsg = "Connection::sendSharedObjectRequest: Unsupported data type '";
            emsg.append(QString::number(static_cast<int>(event.type())));
            emsg.append("'.");
            emit this->error(emsg);
            break;
        }
    }

    // Set up the connection so that the shared object manages itself.
    if (events[0].type() == CPSharedEventType::Use) {
        QObject::connect(this, &CPConnection::sharedObjectChanged, so, &CPSharedObject::synchronize);
    } else if (events[0].type() == CPSharedEventType::Release) {
        so->deleteLater();
        QObject::disconnect(so, 0, 0, 0);
    }

    this->write(buf);
}

bool CPConnection::enterStage(const QString &stage, CPSharedObject::Character charId)
{
    const CPStageInfo *info = CPStageInfo::fromStageId(stage);
    if (info == nullptr) {
        return false;
    }

    QByteArray soName = "share_";
    soName.append(info->name());

    if (this->m_sharedObject != nullptr) {
        if (soName == this->m_sharedObject->name()) {
            return false;
        }
        this->leaveStage();
    }

    this->m_sharedObject = new CPSharedObject(this);
    CPSharedObject *so = this->m_sharedObject;
    so->setName(soName);
    so->setVersion(0);

    QObject::connect(so, &CPSharedObject::playerNameReceived, this, &CPConnection::playerNameReceived);
    QObject::connect(so, &CPSharedObject::playerMessageReceived, this,
                     &CPConnection::playerMessageReceived);
    QObject::connect(so, &CPSharedObject::playerLeft, this, &CPConnection::playerLeft);
    QObject::connect(so, &CPSharedObject::error, this, &CPConnection::error);
    QObject::connect(so, &CPSharedObject::connected, this, &CPConnection::stageEntrySuccessful);
    QObject::connect(so, &CPSharedObject::playerCharacterIdReceived, this,
                     &CPConnection::playerCharacterIdReceived);
    QObject::connect(so, &CPSharedObject::playerDirectionChanged, this,
                     &CPConnection::playerDirectionChanged);
    QObject::connect(so, &CPSharedObject::playerMessagePositionChanged, this,
                     &CPConnection::playerMessagePositionChanged);
    QObject::connect(so, &CPSharedObject::playerPositionChanged, this,
                     &CPConnection::playerPositionChanged);

    CPSharedEvent use;
    use.setType(CPSharedEventType::Use);
    use.setDataSize();

    QVector<CPSharedEvent> events;
    events.append(use);

    this->sendSharedObjectRequest(so, events);

    // Write the command object
    CPCodec buf;
    quint32 messageSize = 86 + CPCodec::amfStringSize(info->name());

    this->writeHeader(buf, 3, 0, CPMessageType::Amf0Command, messageSize, this->currentTime());

    buf.appendAmfString("manageLoginStage");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfString(info->name());
    buf.appendAmfString(QByteArray());
    buf.appendAmfNumber(static_cast<double>(info->direction()));
    buf.appendAmfNumber(info->xPos());
    buf.appendAmfNumber(info->yPos());
    buf.appendAmfNumber(static_cast<double>(CPSharedObject::Direction::Left));
    buf.appendAmfNumber(static_cast<double>(charId));
    buf.appendAmfNumber(info->speed());

    this->write(buf);
    return true;
}

void CPConnection::onSocketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit this->error(this->m_socket.errorString());
    this->close();
}

void CPConnection::sendClientMessage(const QString &message)
{
    CPCodec buf;
    QByteArray msg = message.toUtf8();
    quint32 messageSize = 26 + CPCodec::amfStringSize(msg);

    this->writeHeader(buf, 3, 0, CPMessageType::Amf0Command, messageSize, this->currentTime());
    buf.appendAmfString("manageMessage");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfString(msg);

    this->write(buf);

    fprintf(stderr, "Message sent, sleeping 2 seconds...\n");
    QThread::sleep(2);
    fprintf(stderr, "I'm woke now!...\n");
}

void CPConnection::sendKeepAlive()
{
    CPCodec buf;

    // use channel 4 for this
    this->writeHeader(buf, 4, 0, CPMessageType::Amf0Command, 32, this->currentTime());
    buf.appendAmfString("manageLastAliveTime");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));

    this->write(buf);
}

void CPConnection::leaveStage(bool rula)
{
    if (this->m_sharedObject == nullptr) {
        return;
    }

    CPCodec buf;
    QByteArray command;
    int chan;
    int msgSize;
    // Use channel 5
    if (rula) {
        command = "manageRula";
        chan = 5;
        msgSize = 23;
    } else {
        command = "manageLogoutStage";
        chan = 8;
        msgSize = 30;
    }

    this->writeHeader(buf, chan, 0, CPMessageType::Amf0Command, msgSize, this->currentTime());
    buf.appendAmfString(command);
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    this->write(buf);

    CPSharedEvent release;
    release.setType(CPSharedEventType::Release);
    release.setDataSize();

    QVector<CPSharedEvent> events;
    events.append(release);
    sendSharedObjectRequest(this->m_sharedObject, events);
}

void CPConnection::requestStageLoginInfo()
{
    CPCodec buf;

    // send this on channel 5.
    this->writeHeader(buf, 6, 0, CPMessageType::Amf0Command, 34, this->currentTime());
    buf.appendAmfString("manageStageLoginCount");
    int txid = this->m_transactions.fetchAndAddOrdered(1);
    buf.appendAmfNumber(txid);
    buf.append(static_cast<char>(CPAmfType::Null));
    this->m_rpcs.append(CPRpc("manageStageLoginCount", txid));
    this->write(buf);
}

void CPConnection::onStageLoginInfo(const QByteArray &res, int i)
{

    const CPCodec &buf = this->m_activeChannel->constBuffer();

    // This is probably unneeded.
    if (res != "_result") {
        QString emsg = "<big>IF YOU SEE THIS MESSAGE WHILE SCROLLING THE CHAT LOG</big></br>";
        emsg += "You have been visited by the error message of manageStageLoginCount</br>";
        emsg += "Much hapiness and good feels will come to you</br>";
        emsg += "if you foward this message to the developers, in its entirety.</br>";
        emsg += "Yes that includes the following bit:</br>\t";
        emsg += QString::fromUtf8(QByteArray(&buf.constData()[i], buf.size() - i).toHex());
        emit this->error(emsg);
        return;
    }

    // Skip NULL and array header
    i += 6;
    QStringList names;
    QStringList broadcasters;
    QVector<int> loginCounts;

    // There is an End of object marker at the end. skip that as well
    while (i < buf.size() - 3) {
        QByteArray name;
        QByteArray broadcaster;
        double loginCount;

        if (buf.decodeString(&i, &name) != CPCodec::Status::Ok) {
            goto error_cond;
        }

        // Skip object header and property name "Broadcaster"
        i += 14;
        if (buf.decodeAmfString(&i, &broadcaster) != CPCodec::Status::Ok) {
            goto error_cond;
        }

        // Skip the property name "LoginCount"
        i += 12;
        if (buf.decodeAmfNumber(&i, &loginCount) != CPCodec::Status::Ok) {
            goto error_cond;
        }

        // Skip object end marker
        i += 3;
        names.append(QString::fromUtf8(name));
        broadcasters.append(QString::fromUtf8(broadcaster));
        loginCounts.append(loginCount);
    }

    emit this->stageLoginInfoReceived(names, broadcasters, loginCounts);

    if (0) {
error_cond:
        emit this->error("Connection::onStageLoginInfo: Could not decode object");
    }
    return;
}

void CPConnection::close()
{
    QObject::disconnect(&this->m_socket, &QIODevice::readyRead, 0, 0);
    this->m_timer.stop();
    this->m_socket.close();
}

void CPConnection::ignorePlayer(int playerId)
{
    CPCodec buf;

    // use channel 7
    this->writeHeader(buf, 7, 0, CPMessageType::Amf0Command, 38, this->currentTime());
    buf.appendAmfString("manageIgnoreEach");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfNumber(playerId);
    return;
}

void CPConnection::onIgnored(const QByteArray &command, int index)
{
    Q_UNUSED(command);

    const CPCodec &buf = this->m_activeChannel->buffer();
    double playerId;
    index = 29;
    buf.decodeAmfNumber(&index, &playerId);

    emit this->ignored(playerId);
}

void CPConnection::onAnnouncement(const QByteArray &command, int index)
{
    const CPCodec &buf = this->m_activeChannel->buffer();
    index = 29;
    QByteArray msg;
    buf.decodeAmfString(&index, &msg);

    emit this->announcement(0, QString::fromUtf8(msg));
}

void CPConnection::onManageEventList(const QByteArray &command, int i)
{
    Q_UNUSED(command);
    i = 28;


    const CPCodec &buf = this->m_activeChannel->buffer();
    QStringList names;
    QStringList users;
    QStringList dates;
    QStringList ids;
    QStringList ratings;
    QStringList times;
    QStringList stages;

    if (buf.constData()[i++] != static_cast<char>(CPAmfType::MixedArray)) {
        // emit this->error();
        return;
    }

    quint32 size;
    buf.decodeInt32(&i, &size);

    while (i < buf.size() - 3 && size > 0) {
        // Skip array index name
        quint16 nameSize;
        buf.decodeInt16(&i, &nameSize);
        i += nameSize;

        // skip start of object marker
        i += 1;

        {
            // skip property name 'event_date'
            i += 12;
            QByteArray date;
            if (buf.decodeAmfString(&i, &date) != CPCodec::Status::Ok) {
                goto error_cond;
            }
            dates += QString::fromUtf8(date);
        }

        {
            // skip property name 'event_no'
            i += 10;
            QByteArray id;
            if (buf.decodeAmfString(&i, &id) != CPCodec::Status::Ok) {
                goto error_cond;
            }
            ids += QString::fromUtf8(id);
        }

        {
            // skip property name 'popularity'
            i += 12;
            QByteArray pop;
            if (buf.decodeAmfString(&i, &pop) != CPCodec::Status::Ok) {
                goto error_cond;
            }
            ratings += QString::fromUtf8(pop);
        }

        {
            // skip property name 'reguser_name'
            i += 14;
            QByteArray user;
            if (buf.decodeAmfString(&i, &user) != CPCodec::Status::Ok) {
                goto error_cond;
            }
            users += QString::fromUtf8(user);

        }

        {
            // skip property name 'stage_id'
            i += 10;
            QByteArray stage;
            if (buf.decodeAmfString(&i, &stage) != CPCodec::Status::Ok) {
                goto error_cond;
            }
            stages += QString::fromUtf8(stage);
        }

        {
            // skip property name 'starttime'
            i += 11;
            QByteArray t;
            if (buf.decodeAmfString(&i, &t) != CPCodec::Status::Ok) {
                goto error_cond;
            }
            times += QString::fromUtf8(t);
        }


        {
            // Skip property name 'title'
            i += 7;
            QByteArray t;
            if (buf.decodeAmfString(&i, &t) != CPCodec::Status::Ok) {
                goto error_cond;
            }
            names += QString::fromUtf8(t);
        }

        // skip end of object marker.
        i += 3;
    }

    emit this->eventListReceived(ids, names, dates, times, stages, users, ratings);
    if (0) {
error_cond:
        this->error("Could not decode Event List");
    }
    return;
}

void CPConnection::queryEventList(const QDateTime *datetime,
                                  const QString *name,
                                  const QString *stage,
                                  int rating)
{
    QByteArray d;
    QByteArray t;
    QByteArray n;
    QByteArray s;

    // Should these be converted to JST? Use UTC for now.
    if (datetime == nullptr) {
        QDateTime now = QDateTime::currentDateTime().toUTC();
        d = now.toString("yyyy/MM/dd").toUtf8();
        t = now.toString("HH:mm:00").toUtf8();
    } else {
        QDateTime dt = datetime->toUTC();
        d = dt.toString("yyyy/MM/dd").toUtf8();
        t = dt.toString("HH:mm:00").toUtf8();
    }

    if (name != nullptr) {
        n = name->toUtf8();
    }

    if (stage != nullptr) {
        s = stage->toUtf8();
    }

    rating = (rating < 0) ? 0 : rating;

    quint32 msgSize = 34;
    msgSize += CPCodec::amfStringSize(d);
    msgSize += CPCodec::amfStringSize(t);
    msgSize += CPCodec::amfStringSize(n);
    msgSize += CPCodec::amfStringSize(s);
    msgSize += CPCodec::amfNumberSize();

    CPCodec buf;
    this->writeHeader(buf, 9, 0, CPMessageType::Amf0Command, msgSize, this->currentTime());
    buf.appendAmfString("manageSearchEventList");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfString(d);
    buf.appendAmfString(t);
    buf.appendAmfString(n);
    buf.appendAmfString(s);
    buf.appendAmfNumber(rating);
    this->write(buf);
}

void CPConnection::voteEvent(const QString &eventId)
{
    quint32 msgSize = 28;
    QByteArray eid = eventId.toUtf8();

    msgSize += CPCodec::amfStringSize(eid);

    CPCodec buf;
    this->writeHeader(buf, 10, 0, CPMessageType::Amf0Command, msgSize, this->currentTime());

    buf.appendAmfString("manageVoteEvent");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfString(eid);

    this->write(buf);
}

void CPConnection::onManageVoteEvent(const QByteArray &command, int i)
{
    Q_UNUSED(command);
    i = 28;

    const CPCodec &buf = this->m_activeChannel->constBuffer();

    QByteArray res;
    buf.decodeAmfString(&i, &res);

    bool accepted = false;
    if (res == "success") {
        accepted = true;
    }
    emit this->eventVoteStatusReceived(accepted);
}

void CPConnection::deleteEvent(const QString &eventId)
{
    QByteArray eid = eventId.toUtf8();
    quint32 msgSize = 30;
    msgSize += CPCodec::amfStringSize(eid);
    CPCodec buf;
    this->writeHeader(buf, 11, 0, CPMessageType::Amf0Command, msgSize, this->currentTime());
    buf.appendAmfString("manageDeleteEvent");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfString(eid);

    this->write(buf);
}

void CPConnection::onManageDeleteEvent(const QByteArray &command, int i)
{
    Q_UNUSED(command);
    i = 30;

    const CPCodec &buf = this->m_activeChannel->constBuffer();
    QByteArray res;
    buf.decodeAmfString(&i, &res);

    emit this->eventDeleted(true);
}

bool CPConnection::registerEvent(const QString &name,
                                 const QDateTime &datetime,
                                 const QString &stageId,
                                 const QString &description,
                                 const QString &imageUrl,
                                 const QString &website,
                                 const QString &username,
                                 const QString &password)
{
    quint32 msgSize = 32;

    // What happens if an invalid sid sent to the server? an onStatus command?
    QByteArray s = stageId.toUtf8();
    if (CPStageInfo::fromStageId(s) == nullptr) {
        return false;
    }

    msgSize += CPCodec::amfStringSize(s);

    QByteArray n = name.toUtf8();
    msgSize += CPCodec::amfStringSize(n);

    QByteArray d = description.toUtf8();
    msgSize += CPCodec::amfStringSize(d);

    QByteArray i = imageUrl.toUtf8();
    msgSize += CPCodec::amfStringSize(i);

    QByteArray w = website.toUtf8();
    msgSize += CPCodec::amfStringSize(w);

    QByteArray u = username.toUtf8();
    msgSize += CPCodec::amfStringSize(u);

    QByteArray p = password.toUtf8();
    msgSize += CPCodec::amfStringSize(p);

    QDateTime dt = datetime.toUTC();
    QByteArray date = dt.toString("yyyy/MM/dd").toUtf8();
    msgSize += CPCodec::amfStringSize(date);

    QByteArray t = dt.toString("HH:mm:00").toUtf8();
    msgSize += CPCodec::amfStringSize(t);

    CPCodec buf;
    this->writeHeader(buf, 12, 0, CPMessageType::Amf0Command, msgSize, this->currentTime());

    buf.appendAmfString("manageRegisterEvent");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfString("");
    buf.appendAmfString(date);
    buf.appendAmfString(t);
    buf.appendAmfString(n);
    buf.appendAmfString(s);
    buf.appendAmfString(d);
    buf.appendAmfString(i);
    buf.appendAmfString(w);
    buf.appendAmfString(u);
    buf.appendAmfString(p);

    this->write(buf);
    return true;
}

void CPConnection::onManageRegisterEvent(const QByteArray &command, int i)
{
    Q_UNUSED(command);
    i = 32;

    const CPCodec &buf = this->m_activeChannel->constBuffer();
    QByteArray res;
    buf.decodeAmfString(&i, &res);

    emit this->eventRegistered((res == "true"));
}

void CPConnection::queryEventDetails(const QString &eventId)
{
    QByteArray e = eventId.toUtf8();
    quint32 msgSize = 36;
    msgSize += CPCodec::amfStringSize(e);

    CPCodec buf;
    this->writeHeader(buf, 13, 0, CPMessageType::Amf0Command, msgSize, this->currentTime());
    buf.appendAmfString("manageSearchEventDetail");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfString(e);
    this->write(buf);
}

void CPConnection::onManageEventDetails(const QByteArray &command, int i)
{
    Q_UNUSED(command);
    i = 30;

    const CPCodec &buf = this->m_activeChannel->constBuffer();
    QString datetime;
    QByteArray a;

    {
        QByteArray t;

        buf.decodeAmfString(&i, &a);
        t += a;
        buf.decodeAmfString(&i, &a);
        t += a;
        buf.decodeAmfString(&i, &a);
        t += a;
        buf.decodeAmfString(&i, &a);
        t += a;
        buf.decodeAmfString(&i, &a);
        t += a;

        datetime = QString::fromUtf8(t);
    }

    buf.decodeAmfString(&i, &a);
    QString stage = QString::fromUtf8(a);

    buf.decodeAmfString(&i, &a);
    QString name = QString::fromUtf8(a);

    buf.decodeAmfString(&i, &a);
    QString des = QString::fromUtf8(a);

    buf.decodeAmfString(&i, &a);
    QString img = QString::fromUtf8(a);

    buf.decodeAmfString(&i, &a);
    QString web = QString::fromUtf8(a);

    buf.decodeAmfString(&i, &a);
    QString passwd = QString::fromUtf8(a);

    buf.decodeAmfString(&i, &a);
    QString user = QString::fromUtf8(a);

    emit this->eventDetailsReceived(name,
                                    stage,
                                    QDateTime::fromString(datetime, "yyyyMMddHHmm"),
                                    img,
                                    web,
                                    user,
                                    passwd);
}


void CPConnection::sendDirection(int xpos, int ypos, CPSharedObject::Direction d)
{
    CPCodec buf;
    this->writeHeader(buf, 14, 0, CPMessageType::Amf0Command, 55, this->currentTime());
    buf.appendAmfString("manageDirection");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfNumber(static_cast<int>(d));
    buf.appendAmfNumber(xpos);
    buf.appendAmfNumber(ypos);
    this->write(buf);
}

void CPConnection::sendPosition(int xpos, int ypos, CPSharedObject::Direction d)
{
    CPCodec buf;
    this->writeHeader(buf, 16, 0, CPMessageType::Amf0Command, 54, this->currentTime());
    buf.appendAmfString("managePosition");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfNumber(static_cast<int>(d));
    buf.appendAmfNumber(xpos);
    buf.appendAmfNumber(ypos);
    this->write(buf);
}

void CPConnection::sendMessagePosition(CPSharedObject::MessagePosition p)
{
    CPCodec buf;
    this->writeHeader(buf, 17, 0, CPMessageType::Amf0Command, 43, this->currentTime());
    buf.appendAmfString("manageMessagePosition");
    buf.appendAmfNumber(0);
    buf.append(static_cast<char>(CPAmfType::Null));
    buf.appendAmfNumber(static_cast<int>(p));
    this->write(buf);
}

CPChannel *CPConnection::channel(ChunkCache &cache, int channel)
{
    if (channel < 2 || channel > 65599) {
        return nullptr;
    }

    if (channel - 2 >= cache.size()) {
        cache.resize(channel - 1);
    }

    return &cache[channel - 2];
}
