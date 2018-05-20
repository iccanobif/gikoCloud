#ifndef CUTIEPOI_NET_MESSAGE_TYPE_HPP
#define CUTIEPOI_NET_MESSAGE_TYPE_HPP

enum class CPMessageType : quint8 {
    None = 0,

    ChunkSize = 1,
    Abort = 2,
    Aknowledgement = 3,
    UserControlMessage = 4,
    WindowAcknowledgementSize = 5,
    PeerBandwidth = 6,

    AudioData = 8,
    VideoData = 9,

    Amf3MetaData = 15,
    Amf3SharedObject = 16,
    Amf3Command = 17,

    Amf0MetaData = 18,
    Amf0SharedObject = 19,
    Amf0Command = 20,

    Aggregate = 22,
};

#endif // CUTIEPOI_NET_MESSAGE_TYPE_HPP
