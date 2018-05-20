#include "channel.hpp"

CPChannel::CPChannel() :
    m_buffer(),
    m_messageSize(0),
    m_timestampDelta(0),
    m_timestamp(0),
    m_extendedTimestamp(0),
    m_streamId(0),
    m_messageType(CPMessageType::None),
    m_hasExtendedTimestamp(false)
{
}

void CPChannel::reset()
{
    this->buffer().resize(0);
    this->setMessageSize(0);
    this->setTimestampDelta(0);
    this->setTimestamp(0);
    this->setStreamId(0);
    this->setMessageType(CPMessageType::None);
}

void CPChannel::done()
{
    this->buffer().resize(0);
}
