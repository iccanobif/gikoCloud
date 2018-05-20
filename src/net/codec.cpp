#include <QtEndian>
#include <QString>
#include <QStack>
#include "codec.hpp"

union UNum {
    double dval;
    quint64 qval;
    qint64 iqval;
};

void CPCodec::appendInt16(quint16 n)
{
    quint8 buf[2];

    buf[0] = (n >> 0x08) & 0xFF;
    buf[1] = (n >> 0x00) & 0xFF;

    this->append(reinterpret_cast<const char *>(buf), 2);
}

void CPCodec::appendInt24(quint32 n)
{
    quint8 buf[3];

    buf[0] = (n >> 0x10) & 0xFF;
    buf[1] = (n >> 0x08) & 0xFF;
    buf[2] = (n >> 0x00) & 0xFF;

    this->append(reinterpret_cast<const char *>(buf), 3);
}

static void encodeInt32(quint8 *buf, quint32 n)
{
    buf[0] = (n >> 0x18) & 0xFF;
    buf[1] = (n >> 0x10) & 0xFF;
    buf[2] = (n >> 0x08) & 0xFF;
    buf[3] = (n >> 0x00) & 0xFF;
}

void CPCodec::appendInt32(quint32 n)
{
    quint8 buf[4];
    encodeInt32(buf, n);
    this->append(reinterpret_cast<const char *>(buf), 4);
}

void CPCodec::appendInt32Le(quint32 n)
{
    quint8 buf[4];

    buf[0] = (n >> 0x00) & 0xFF;
    buf[1] = (n >> 0x08) & 0xFF;
    buf[2] = (n >> 0x10) & 0xFF;
    buf[3] = (n >> 0x18) & 0xFF;

    this->append(reinterpret_cast<const char *>(buf), 4);
}

void CPCodec::appendString(const QByteArray &str)
{
    quint16 length = str.size();
    this->appendInt16(length);
    this->append(str.constData(), length);
}

CPCodec::Status CPCodec::decodeInt16(int *pos, quint16 *dest) const
{
    const quint8 *data;
    int i = *pos;

    if (this->size() < i + 2) {
        return Status::InvalidObject;
    }

    data = reinterpret_cast<const quint8 *>(this->constData());
    *dest = (data[i] << 0x08) | (data[i + 1] << 0x00);
    i += 2;
    *pos = i;

    return Status::Ok;
}

CPCodec::Status CPCodec::decodeInt24(int *pos, quint32 *dest) const
{
    const quint8 *data;
    int i = *pos;

    if (this->size() < i + 3) {
        return Status::InvalidObject;
    }

    data = reinterpret_cast<const quint8 *>(this->constData());
    *dest = 0;

    *dest |= (data[i + 0] << 0x10);
    *dest |= (data[i + 1] << 0x08);
    *dest |= (data[i + 2] << 0x00);
    i += 3;
    *pos = i;

    return Status::Ok;
}

CPCodec::Status CPCodec::decodeInt32(int *pos, quint32 *dest) const
{
    const quint8 *data;
    int i = *pos;

    if (this->size() < i + 4) {
        return Status::InvalidObject;
    }

    data = reinterpret_cast<const quint8 *>(this->constData());
    *dest = 0;

    *dest |= (data[i + 0] << 0x18) | (data[i + 1] << 0x10);
    *dest |= (data[i + 2] << 0x08) | (data[i + 3] << 0x00);
    i += 4;
    *pos = i;

    return Status::Ok;
}

CPCodec::Status CPCodec::decodeInt32Le(int *pos, quint32 *dest) const
{
    const quint8 *data;
    int i = *pos;

    if (this->size() < i + 4) {
        return Status::InvalidObject;
    }

    data = reinterpret_cast<const quint8 *>(this->constData());
    *dest = 0;

    *dest |= (data[i + 0] << 0x00) | (data[i + 1] << 0x08);
    *dest |= (data[i + 2] << 0x10) | (data[i + 3] << 0x18);
    i += 4;
    *pos = i;

    return Status::Ok;
}

CPCodec::Status CPCodec::decodeString(int *pos, QByteArray *dest) const
{
    quint16 length;
    Status res;
    int i = *pos;

    if ((res = this->decodeInt16(&i, &length)) != Status::Ok) {
        return res;
    }

    if (this->size() < i + length) {
        return Status::InvalidObject;
    }

    *dest = QByteArray(&this->constData()[i], length);
    i += length;
    *pos = i;

    return Status::Ok;
}

void CPCodec::appendAmfString(const QByteArray &str)
{
    quint8 buf[5];
    quint32 length = str.size();

    if (length > 0xFFFF) {
        buf[0] = static_cast<quint8>(CPAmfType::LongString);
        buf[1] = (length >> 0x18) & 0xFF;
        buf[2] = (length >> 0x10) & 0xFF;
        buf[3] = (length >> 0x08) & 0xFF;
        buf[4] = (length >> 0x00) & 0xFF;
        this->append(reinterpret_cast<const char *>(buf), 5);
    } else {
        buf[0] = static_cast<quint8>(CPAmfType::String);
        buf[1] = (length >> 0x08) & 0xFF;
        buf[2] = (length >> 0x00) & 0xFF;
        this->append(reinterpret_cast<const char *>(buf), 3);
    }

    this->append(str.constData(), str.size());
}

void CPCodec::appendAmfNumber(double d)
{
    UNum num;
    uchar bytes[9];

    num.dval = d;
    qToBigEndian(num.qval, &bytes[1]);
    bytes[0] = static_cast<uchar>(CPAmfType::Number);

    this->append(reinterpret_cast<const char *>(bytes), 9);
}

void CPCodec::appendAmfBoolean(bool b)
{
    char buf[2];

    buf[0] = static_cast<char>(CPAmfType::Boolean);
    buf[1] = b;

    this->append(buf, 2);
}

void CPCodec::appendAmfObject(const CPAmfObject &obj)
{
    QStack<CPAmfObject::const_iterator> itrStack;
    QStack<const CPAmfObject *> objStack;
    const CPAmfObject *o = &obj;
    const CPAmfObject *root = o;
    CPAmfObject::const_iterator i = o->begin();

    this->append(static_cast<char>(CPAmfType::Object));

    forever {
        // If there are no more properties, write the end of object
        // signature.
        const CPAmfProperty &p = *i;
        if (i == o->end()) {
            this->appendInt16(0);
            this->append(static_cast<char>(CPAmfType::EndOfObject));

            if (o == root) {
                break;
            } else {
                // Return to the parent's level.
                o = objStack.pop();
                i = itrStack.pop();
                continue;
            }
        }

        // Start encoding properties
        this->appendString(p.constName());

        if (p.type() == CPAmfType::Object) {
            // Increment before saving so that the next child would be encoded
            // when the child object has been encoded.
            ++i;
            itrStack.push(i);
            this->append(static_cast<char>(CPAmfType::Object));

            objStack.push(o);
            o = p.constObject();
            i = o->begin();
        } else {
            // regular properties
            if (p.type() == CPAmfType::Boolean) {
                this->appendAmfBoolean(p.boolean());
            } else if (p.type() == CPAmfType::String) {
                this->appendAmfString(*(p.constString()));
            } else if (p.type() == CPAmfType::Number) {
                this->appendAmfNumber(p.number());
            }

            ++i;
        }
    }
}

CPCodec::Status CPCodec::decodeAmfString(int *pos, QByteArray *dest) const
{
    int i = *pos;
    if (this->size() < i) {
        return Status::InvalidObject;
    }

    quint32 length;
    const char *data = this->constData();
    Status res;

    if (data[i] == static_cast<quint8>(CPAmfType::String)) {
        quint16 len16;
        i += 1;

        if ((res = this->decodeInt16(&i, &len16)) != Status::Ok) {
            return res;
        }

        length = len16;
    } else if (data[i] == static_cast<quint8>(CPAmfType::LongString)) {
        i += 1;

        if ((res = this->decodeInt32(&i, &length)) != Status::Ok) {
            return res;
        }
    } else {
        return Status::InvalidType;
    }

    if (this->size() >= 0 && quint32(this->size()) < (i + length)) {
        return Status::InvalidObject;
    }

    *dest = QByteArray(&data[i], length);
    i += length;
    *pos = i;
    return Status::Ok;
}

CPCodec::Status CPCodec::decodeAmfObject(int *pos, CPAmfObject *dest) const
{
    int i = *pos;
    QStack<CPAmfObject *> objStack;
    CPAmfObject *obj = dest;
    CPAmfObject *root = obj;
    Status res;

    // This is probably what the minimum size of an AmfObject is:
    //      00: AmfType::Object,
    //      01: length = 0,
    //      03: AmfType::EndOfObject
    if (this->size() < 4) {
        return Status::InvalidObject;
    }

    if (this->constData()[i] != static_cast<quint8>(CPAmfType::Object)) {
        return Status::InvalidType;
    }

    i += 1;
    QByteArray name;

    for (;;) {
        // Always make sure that the buffer can hold the EndOfObject
        // Signature
        if (this->size() < 3) {
            return Status::InvalidObject;
        }

        // Decode the name of this property.
        if ((res = this->decodeString(&i, &name)) != Status::Ok) {
            return res;
        }

        // Check if the buffer has bytes remaining
        if (this->size() <= i) {
            return Status::InvalidObject;
        }

        CPAmfType type = static_cast<CPAmfType>(this->constData()[i]);

        if (type == CPAmfType::Number) {
            double num;

            if ((res = this->decodeAmfNumber(&i, &num)) != Status::Ok) {
                return res;
            }

            CPAmfProperty tmp;
            tmp.setName(name);
            tmp.setNumber(num);
            obj->append(std::move(tmp));
        } else if (type == CPAmfType::String
                   || type == CPAmfType::LongString) {
            QByteArray str;

            if ((res = this->decodeAmfString(&i, &str)) != Status::Ok) {
                return res;
            }

            CPAmfProperty tmp;
            tmp.setName(name);
            tmp.setString(str);
            obj->append(std::move(tmp));
        } else if (type == CPAmfType::Object) {
            CPAmfProperty tmp;
            tmp.setName(name);
            tmp.setObject(QVector<CPAmfProperty>());
            obj->append(std::move(tmp));
            objStack.push(obj);
            obj = obj->last().object();

            ++i;
        } else if (type == CPAmfType::EndOfObject) {
            obj->squeeze();
            ++i;

            if (obj == root) {
                *pos = i;
                return Status::Ok;
            } else {
                obj = objStack.pop();
            }
        } else if (type == CPAmfType::Null) {
            ++i;
            CPAmfProperty tmp;
            tmp.setName(name);
            tmp.setNull();
            obj->append(std::move(tmp));
        } else if (type == CPAmfType::Undefined) {
            ++i;
            CPAmfProperty tmp;
            tmp.setName(name);
            tmp.setUndefined();
            obj->append(std::move(tmp));
        } else {
            return Status::UnsupportedType;
        }
    }
}

CPCodec::Status CPCodec::decodeAmfNumber(int *pos, double *dest) const
{
    int i = *pos;
    UNum num;
    const char *data = this->constData();

    if (this->size() < i + 9) {
        return Status::InvalidObject;
    }

    if (data[i] != static_cast<char>(CPAmfType::Number)) {
        return Status::InvalidType;
    }

    memcpy(&num.dval, &data[i + 1], 8);
    num.iqval = qFromBigEndian(num.iqval);

    *dest = num.dval;
    i += 9;
    *pos = i;

    return Status::Ok;
}

CPCodec::Status CPCodec::decodeBoolean(int *pos, bool *b) const
{
    int i = *pos;
    const char *data = this->constData();

    if (this->size() < i + 2) {
        return Status::InvalidObject;
    }

    if (data[i] != static_cast<char>(CPAmfType::Boolean)) {
        return Status::InvalidType;
    }

    *b = data[i + 1];
    i += 2;
    *pos = i;

    return Status::Ok;
}

void CPCodec::updateNextChunkIndex() noexcept
{
    int size = this->size();
    int headerSize = this->headerSize();
    int chunkSize = this->chunkSize();

    // Don't calculate if the next chunk index has not been written to yet.
    // Don't calculate if this is not chunking: the appending methods check if
    // the codec is chunking first, so the value of nextChunkIndex does not
    // matter.
    if (!this->isChunking() || size <= this->nextChunkIndex()) {
        return;
    }

    // The position of the secondary chunk header
    int secondChunk = headerSize + chunkSize;

    if (size  <= secondChunk) {
        this->m_nextChunkIndex = secondChunk;
    } else {
        int nsc = this->numSecondaryChunks();
        // sum of header sizes of secondary chunks
        int hh = (this->channel() <= 63 ? 1 : this->channel() <= 319 ? 2 : 3);
        hh += (this->hasExtendedTimeValue() ? 4 : 0);
        hh *= nsc;
        this->m_nextChunkIndex = secondChunk + hh + chunkSize * nsc;
    }
}

void CPCodec::appendChunkHeader(CPHeaderType hType)
{
    // can't write invalid chunk ids
    int channel = this->channel();
    if (channel < 2) {
        return;
    }

    if (channel <= 63) {
        this->m_bytes.append(static_cast<quint8>(hType) | channel);
    } else {
        char buf[3];

        if (channel <= 319) {
            buf[0] = static_cast<char>(hType);
            buf[1] = (channel - 64) & 0xff;
            this->m_bytes.append(buf, 2);
        } else {
            channel -= 64;
            buf[0] = static_cast<char>(hType) | 1;
            buf[1] = (channel >> 0) & 0xff;
            buf[2] = (channel >> 8) & 0xff;
            this->m_bytes.append(buf, 3);
        }
    }

    this->m_numSecondaryChunks++;
}

void CPCodec::append(char c)
{
    // Update chunk index before appending
    if (this->isChunking()) {
        updateNextChunkIndex();
        if (this->nextChunkIndex() == this->size()) {
            this->appendChunkHeader();
        }
    }

    this->m_bytes.append(c);
}

void CPCodec::append(const char *s, int len)
{
    // Update the chunk index so that the first branch may be taken and an extra
    // chunk header does not get appended.
    if (this->isChunking()) {
        updateNextChunkIndex();
    }
    int nextChunkIndex = this->nextChunkIndex();
    int size = this->size();
    int end = size + len;

    // Only append the whole array if this isn't chunking, or if this->m_bytes[size + len]
    // after appending is before or at the nextChunkIndex. This is correct as it is
    // unknown if there will be more chunks after the current one. If there are,
    // the next time this method is called, as this->size() == this->nextChunkIndex()
    // a chunk header would get appended first.
    if (!this->isChunking() || end <= nextChunkIndex) {
        this->m_bytes.append(s, len);
    } else {
        int bytesWritten = 0;
        // Write until the next chunk
        if (size != nextChunkIndex) {
            int rem = nextChunkIndex - size;
            this->m_bytes.append(s, rem);
            bytesWritten += rem;
            s += rem;
        }

        int chunkSize = this->chunkSize();
        // Write the remainder in chunks.
        do {
            int bytesToWrite = (chunkSize < (len - bytesWritten) ? chunkSize : len - bytesWritten);

            this->appendChunkHeader();
            this->m_bytes.append(s, bytesToWrite);

            s += bytesToWrite;
            bytesWritten += bytesToWrite;
        } while (bytesWritten < len);
    }
}

void CPCodec::append(const QByteArray &a)
{
    this->append(a.constData(), a.size());
}

CPCodec::~CPCodec() noexcept
{
}

CPCodec::CPCodec(const QByteArray &a) noexcept :
    m_extendedTimeValue(0),
    m_numSecondaryChunks(-1),
    m_nextChunkIndex(0),
    m_headerSize(0),
    m_chunkSize(128),
    m_channel(0),
    m_bytes(a),
    m_hasExtendedTimeValue(false)
{
}

CPCodec::CPCodec() noexcept :
    m_extendedTimeValue(0),
    m_numSecondaryChunks(-1),
    m_nextChunkIndex(0),
    m_headerSize(0),
    m_chunkSize(128),
    m_channel(0),
    m_bytes(),
    m_hasExtendedTimeValue(false)
{
}

CPCodec::Status CPCodec::decode(int &i,
                                int &channel,
                                CPHeaderType &hType,
                                quint32 &timestamp,
                                quint32 &mSize,
                                CPMessageType &mType,
                                quint32 &streamId)
{
    const quint8 *cd = reinterpret_cast<const quint8 *>(this->constData());
    channel = cd[i] & 0x3F;
    hType = static_cast<CPHeaderType>(cd[i] & 0xC0);

    if (this->size() >= 18) {
        int hSize;
        if (hType == CPHeaderType::Complete) {
            hSize = 11;
        } else if (hType == CPHeaderType::UpdateData) {
            hSize = 7;
        } else if (hType == CPHeaderType::UpdateTimestamp) {
            hSize = 3;
        } else {
            hSize = 0;
        }

        int bhSize;
        if (channel == 0) {
            channel = cd[i + 1] + 64;
            bhSize = 2;
            i += 2;
        } else if (channel == 1) {
            channel = ((cd[i + 1] + 64) << 0) | ((cd[i + 2] + 64) << 8);
            bhSize = 3;
            i += 3;
        } else {
            bhSize = 1;
            i += 1;
        }

        this->setHeaderSize(hSize + bhSize);
    } else {
        // Need more size for reading
        return Status::InvalidObject;
    }

    if (hType != CPHeaderType::Minimal) {
        this->decodeInt24(&i, &timestamp);
    }

    if (hType < CPHeaderType::UpdateTimestamp) {
        this->decodeInt24(&i, &mSize);
        mType = static_cast<CPMessageType>(cd[i]);
        i += 1;
    }

    if (hType == CPHeaderType::Complete) {
        this->decodeInt32(&i, &streamId);
    }

    if (timestamp > 0xFFFFFF) {
        this->decodeInt32(&i, &timestamp);
    }

    return Status::Ok;
}

CPCodec::Status CPCodec::toAmfObject(int &i, CPAmfObject &dest) const
{
    Status res;
    const char *cd = this->constData();

    CPAmfObject *p = &dest;

    while (i < this->size()) {
        double num;

        if (cd[i] == static_cast<const char>(CPAmfType::Number)) {
            if ((res = this->decodeAmfNumber(&i, &num)) != Status::Ok) {
                return res;
            }

            CPAmfProperty tmp;
            tmp.setNumber(num);
            p->append(std::move(tmp));
        } else if (cd[i] == static_cast<const char>(CPAmfType::Boolean)) {
            bool b;
            if ((res = this->decodeBoolean(&i, &b)) != Status::Ok) {
                return res;
            }

            CPAmfProperty tmp;
            tmp.setBoolean(b);
            p->append(std::move(tmp));
        } else if (cd[i] == static_cast<const char>(CPAmfType::String)
                   || cd[i] == static_cast<const char>(CPAmfType::LongString)) {
            QByteArray str;

            if ((res = this->decodeAmfString(&i, &str)) != Status::Ok) {
                return res;
            }
            CPAmfProperty tmp;
            tmp.setString(str);
            p->append(std::move(tmp));
        } else if (cd[i] == static_cast<const char>(CPAmfType::Object)) {
            CPAmfProperty tmp;
            tmp.setObject(QVector<CPAmfProperty>());

            if ((res = this->decodeAmfObject(&i, tmp.object())) != Status::Ok) {
                return res;
            }

            p->append(std::move(tmp));
        } else {
            return Status::UnsupportedType;
        }
    }

    return Status::Ok;
}

int CPCodec::amfObjectSize(const CPAmfObject &obj)
{
    int sz = 0;

    const CPAmfObject *o = &obj;

    QStack<CPAmfObject::const_iterator> its;
    QStack<const CPAmfObject *> objs;

    CPAmfObject::const_iterator i = o->begin();

    while (i != obj.end()) {
        for (; i != o->end(); ++i) {
            sz += CPCodec::stringSize((*i).constName());
            switch ((*i).type()) {
            case CPAmfType::Number:
                sz += CPCodec::amfNumberSize();
                break;
            case CPAmfType::String:
                sz += CPCodec::amfStringSize(*(*i).constString());
                break;
            case CPAmfType::Boolean:
                sz += CPCodec::amfBooleanSize();
                break;
            case CPAmfType::Object:
                sz += 4;
                objs.push(o);
                its.push(i);
                o = (*i).constObject();
                i = o->begin();
                break;
            default:
                // Unsupported type.
                return -1;
            }
        }

        if (o != &obj) {
            o = objs.pop();
            i = its.pop();
            ++i;
        }
    }

    return sz + 4;
}

const int CPCodec::amfBooleanSize() noexcept
{
    return 2;
}

const int CPCodec::amfNumberSize() noexcept
{
    return 9;
}

int CPCodec::amfStringSize(const QByteArray &str)
{
    return (str.size() <= 65536 ? 3 : 5) + str.size();
}

int CPCodec::stringSize(const QByteArray &str)
{
    return (str.size() < 65535 ? str.size() + 2 : -1);
}

CPCodec::CPCodec(QByteArray &&bytes) noexcept :
    m_extendedTimeValue(0),
    m_numSecondaryChunks(-1),
    m_nextChunkIndex(0),
    m_headerSize(0),
    m_chunkSize(128),
    m_channel(0),
    m_bytes(),
    m_hasExtendedTimeValue(false)
{
    this->m_bytes = std::move(bytes);
}

CPCodec &CPCodec::operator=(const QByteArray &bytes) noexcept
{
    CPCodec tmp(bytes);
    *this = std::move(tmp);
    return *this;
}

CPCodec &CPCodec::operator=(QByteArray &&bytes) noexcept
{
    CPCodec tmp(std::move(bytes));
    *this = std::move(tmp);
    return *this;
}

CPCodec::CPCodec(const CPCodec &other) noexcept :
    m_extendedTimeValue(other.m_extendedTimeValue),
    m_numSecondaryChunks(other.m_numSecondaryChunks),
    m_nextChunkIndex(other.m_nextChunkIndex),
    m_headerSize(other.m_headerSize),
    m_chunkSize(other.m_chunkSize),
    m_channel(other.m_channel),
    m_bytes(other.m_bytes),
    m_hasExtendedTimeValue(other.m_hasExtendedTimeValue)
{
}

CPCodec::CPCodec(CPCodec &&other) noexcept :
    m_extendedTimeValue(other.m_extendedTimeValue),
    m_numSecondaryChunks(other.m_numSecondaryChunks),
    m_nextChunkIndex(other.m_nextChunkIndex),
    m_headerSize(other.m_headerSize),
    m_chunkSize(other.m_chunkSize),
    m_channel(other.m_channel),
    m_bytes(std::move(other.m_bytes)),
    m_hasExtendedTimeValue(other.m_hasExtendedTimeValue)
{
}


CPCodec &CPCodec::operator=(CPCodec &&other) noexcept
{
    if (this != &other) {
        this->m_extendedTimeValue = other.m_extendedTimeValue;
        this->m_numSecondaryChunks = other.m_numSecondaryChunks;
        this->m_nextChunkIndex = other.m_nextChunkIndex;
        this->m_headerSize = other.m_headerSize;
        this->m_chunkSize = other.m_chunkSize;
        this->m_channel = other.m_channel;
        this->m_bytes = std::move(other.m_bytes);
        this->m_hasExtendedTimeValue  = other.m_hasExtendedTimeValue;
    }
    return *this;
}

CPCodec &CPCodec::operator=(const CPCodec &other) noexcept
{
    if (this != &other) {
        CPCodec tmp(other);
        *this = std::move(tmp);
    }
    return *this;
}

#define cp_between(var, min, max)  (var >= min && var <= max)


bool CPCodec::isChunking() const noexcept
{
    return cp_between(this->channel(), 2, 65599) && cp_between(this->chunkSize(), 1, INT_MAX)
           && cp_between(this->headerSize(), 1, 18);
}

void CPCodec::appendChunkHeader()
{
    if (!this->isChunking()) {
        return;
    }
    this->appendChunkHeader(CPHeaderType::Minimal);
    if (this->hasExtendedTimeValue()) {
        quint8 buf[4];
        encodeInt32(buf, this->extendedTimeValue());
        this->m_bytes.append(reinterpret_cast<const char *>(buf), 4);
    }
}
