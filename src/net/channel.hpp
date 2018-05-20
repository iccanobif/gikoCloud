#ifndef CUTIEPOI_NET_CHANNEL_HPP
#define CUTIEPOI_NET_CHANNEL_HPP

#include "codec.hpp"

class CPChannel {
public:
    CPChannel();

    void reset();
    void done();

    inline void setBuffer(CPCodec &codec);
    inline void setExtendedTimestamp(quint32 extendedTimestamp);
    inline void setHasExtendedTimestamp(bool hasExtendedTimestamp);
    inline void setMessageSize(quint32 messageSize);
    inline void setMessageType(CPMessageType messageType);
    inline void setStreamId(quint32 streamId);
    inline void setTimestamp(quint64 timestamp);
    inline void setTimestampDelta(quint64 timestampDelta);

    inline CPCodec &buffer();
    inline const CPCodec &constBuffer() const;
    inline quint32 bytesPending() const;
    inline bool hasExtendedTimestamp() const;
    /**
     * Returns the extended timestamp or timestamp delta for this channel.
     */
    inline quint32 extendedTimeValue() const noexcept;

    inline quint32 messageSize() const;
    inline CPMessageType messageType() const;
    inline quint32 streamId() const;
    inline quint64 timestamp() const;
    inline quint64 timestampDelta() const;

private:
    CPCodec m_buffer;
    quint32 m_messageSize;
    quint32 m_timestampDelta;
    quint32 m_timestamp;
    quint32 m_extendedTimestamp;
    quint32 m_streamId;
    CPMessageType m_messageType;
    bool m_hasExtendedTimestamp;
};

inline void CPChannel::setBuffer(CPCodec &codec)
{
    this->m_buffer = std::move(codec);
}

inline void CPChannel::setHasExtendedTimestamp(bool hasExtendedTimestamp)
{
    this->m_hasExtendedTimestamp = hasExtendedTimestamp;
}

inline void CPChannel::setMessageType(CPMessageType messageType)
{
    this->m_messageType = messageType;
}

inline void CPChannel::setMessageSize(quint32 messageSize)
{
    this->m_messageSize = messageSize;
}

inline void CPChannel::setStreamId(quint32 streamId)
{
    this->m_streamId = streamId;
}

inline void CPChannel::setTimestamp(quint64 timestamp)
{
    this->m_timestamp = timestamp;
}

inline void CPChannel::setTimestampDelta(quint64 timestampDelta)
{
    this->m_timestampDelta = timestampDelta;
}

inline CPCodec &CPChannel::buffer()
{
    return this->m_buffer;
}

inline const CPCodec &CPChannel::constBuffer() const
{
    return this->m_buffer;
}

inline quint32 CPChannel::bytesPending() const
{
    return (this->m_messageSize - this->constBuffer().size());
}

inline bool CPChannel::hasExtendedTimestamp() const
{
    return this->m_hasExtendedTimestamp;
}

inline quint32 CPChannel::messageSize() const
{
    return this->m_messageSize;
}

inline CPMessageType CPChannel::messageType() const
{
    return this->m_messageType;
}

inline quint32 CPChannel::streamId() const
{
    return this->m_streamId;
}

inline quint64 CPChannel::timestamp() const
{
    return this->m_timestamp;
}

inline quint64 CPChannel::timestampDelta() const
{
    return this->m_timestampDelta;
}

inline void CPChannel::setExtendedTimestamp(quint32 extendedTimestamp)
{
    this->m_extendedTimestamp = extendedTimestamp;
}

inline quint32 CPChannel::extendedTimeValue() const noexcept
{
    return this->m_extendedTimestamp;
}

#endif // CUTIEPOI_NET_CHANNEL_HPP
