#ifndef CUTIEPOI_NET_USER_CONTROL_EVENT_HPP
#define CUTIEPOI_NET_USER_CONTROL_EVENT_HPP

#include <QtGlobal>

enum CPUserControlEvent : quint16 {
    StreamBegin,
    StreamEof,
    StreamDry,
    SetBufferLength,
    StreamIsRecorded,
    PingRequest = 6,
    PingResponse
};

#endif // CUTIEPOI_NET_USER_CONTROL_EVENT_HPP
