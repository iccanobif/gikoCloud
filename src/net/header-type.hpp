#ifndef CUTIEPOI_NET_HEADER_TYPE_HPP
#define CUTIEPOI_NET_HEADER_TYPE_HPP

enum class CPHeaderType : quint8 {
    Complete = (0 << 6),
    UpdateData = (1 << 6),
    UpdateTimestamp = (2 << 6),
    Minimal = (3 << 6),
    Invalid = 0x3F
};

#endif // CUTIEPOI_NET_HEADER_TYPE_HPP
