#ifndef CUTIEPOI_NET_CODEC_HPP
#define CUTIEPOI_NET_CODEC_HPP

#include <QByteArray>
#include "amf.hpp"
#include "header-type.hpp"
#include "message-type.hpp"

class CPCodec {
public:
    enum class Status : quint8 {
        Ok,
        InvalidObject,
        InvalidType,
        UnsupportedType
    };

    CPCodec() noexcept;
    /**
     * Copy-consruct a codec.
     */
    CPCodec(const CPCodec &other) noexcept;
    /**
     * Copy-assign a codec to this.
     */
    CPCodec &operator=(const CPCodec &other) noexcept;
    /**
     * Move-construct a codec.
     */
    CPCodec(CPCodec &&other) noexcept;
    /**
     * Move-assign a codec to this.
     */
    CPCodec &operator=(CPCodec &&other) noexcept;


    /**
     * Copy-convert a QByteArray into a codec. The buffer is assumed to be
     * unchunked.
     */
    CPCodec(const QByteArray &a) noexcept;
    /**
     * Copy-assign a QByteArray into this. The buffer is assumed to be
     * unchunked. The codec state is reset with this operator.
     */
    CPCodec &operator=(const QByteArray &bytes) noexcept;
    /**
     * Move-convert a QByteArray into a codec. The buffer is assumed to be
     * unchunked.
     */
    CPCodec(QByteArray &&bytes) noexcept;
    /**
     * Move-assign a QByteArray into this. The buffer is assumed to be
     * unchunked. The codec state is reset with this operator.
     */
    CPCodec &operator=(QByteArray &&bytes) noexcept;
    /**
     * Cast to QByteArray.
     * @return the internal buffer.
     */
    inline operator const QByteArray &() const noexcept;

    /**
     * Destroy a codec.
     */
    ~CPCodec() noexcept;

    // basic appending
    void append(const char *s, int len);
    void append(char c);
    void append(const QByteArray &a);

    // appenders
    void appendInt16(quint16 n);
    void appendInt24(quint32 n);
    void appendInt32(quint32 n);
    void appendInt32Le(quint32 n);
    void appendString(const QByteArray &str);
    void appendAmfBoolean(bool b);
    void appendAmfNumber(double d);
    void appendAmfObject(const CPAmfObject &obj);
    void appendAmfString(const QByteArray &str);


    /**
     * Append the basic header.
     * @param[in] hType The header type.
     */
    void appendChunkHeader(CPHeaderType hType);
    /**
     * Appends a type 3 header. Does nothing if the codec isn't in chunking mode.
     */
    void appendChunkHeader();
    /**
     * Updates the next chunk index. Does nothing if the codec isn't in chunking mode.
     */
    void updateNextChunkIndex() noexcept;

    // decoders
    Status decodeBoolean(int *i, bool *b) const;
    Status decodeInt16(int *i, quint16 *dest) const;
    Status decodeInt24(int *i, quint32 *dest) const;
    Status decodeInt32(int *i, quint32 *dest) const;
    Status decodeInt32Le(int *i, quint32 *val) const;
    Status decodeString(int *i, QByteArray *dest) const;
    Status decodeAmfNumber(int *i, double *dest) const;
    Status decodeAmfString(int *i, QByteArray *dest) const;
    Status decodeAmfObject(int *i, CPAmfObject *dest) const;

    Status decode(int &i, int &channel, CPHeaderType &hType, quint32 &timestamp, quint32 &mSize,
                  CPMessageType &mType, quint32 &streamId);

    Status toAmfObject(int &i, CPAmfObject &dest) const;

    // functions that work with the buffer
    /**
     * Resize the buffer to size bytes. The codec is in an unspecified chunking
     * state after a call to this method.
     * @param[in] size The new size.
     */
    inline void resize(int size);
    /**
     * Clears the buffer. Also resets the headerSize, numSecondaryChunks,
     * nextChunkIndex to their default values (0, -1, -1). The codec is not in
     * chunking mode after a call to this method, but will start chunking
     * once the headerSize has been set.
     */
    inline void clear();
    /**
     * Removes len number of bytes starting at pos from the buffer. The codec
     * is at an unspecified chunking state after a call to this method.
     * @param[in] pos The position to start at.
     * @param[in] len The number of bytes to remove.
     */
    inline void remove(int pos, int len);

    inline int size() const;
    inline char *data();
    inline const char *constData() const;

    // Getters
    inline int nextChunkIndex() const noexcept;
    inline int channel() const noexcept;
    inline int headerSize() const noexcept;
    inline int chunkSize() const noexcept;
    inline int numSecondaryChunks() const noexcept;
    inline bool hasExtendedTimeValue() const noexcept;
    inline quint32 extendedTimeValue() const noexcept;
    /**
     * Check if the codec is in chunking mode. A codec is in chunking mode if
     * it has a valid channel and a valid chunkSize and a headerSize.
     * @return true if the codec is in chunking mode.
     */
    bool isChunking() const noexcept;

    // Setters
    inline void setChannel(int channel) noexcept;
    inline void setChunkSize(int chunkSize) noexcept;
    inline void setHeaderSize(int headerSize) noexcept;
    /**
     * Set hasExtendedTimeValue flag. Setting this flag does not change the
     * chunking mode of a codec. However, if hasExtendedTimeValue is set to true,
     * a chunking codec appends the extended time value after chunk headers.
     *
     * @param[in] hasExtendedTimeValue The new value
     */
    inline void setHasExtendedTimeValue(bool hasExtendedTimeValue) noexcept;
    /**
     * Set the extended time value for this codec. Having an extended time value
     * does not change the chunking mode of a codec. If the hasExtendedTimeValue
     * flag is set to true, the codec would append the extended time value after
     * chunk headers.
     *
     * @param[in] extendedTimeValue The extended timestamp or timestamp delta.
     */
    inline void setExtendedTimeValue(quint32 extendedTimeValue) noexcept;

    /**
     * Returns the size of an Amf Number
     */
    static const int amfNumberSize() noexcept;
    /**
     * Returns the size of an Amf Boolean
     */
    static const int amfBooleanSize() noexcept;
    /**
     * Returns the size of a regular string.
     */
    static int stringSize(const QByteArray &string);
    /**
     * Returns the size of an amf string
     */
    static int amfStringSize(const QByteArray &string);
    /**
     * Returns the size of an Amf Object.
     */
    static int amfObjectSize(const CPAmfObject &object);
private:
    quint32 m_extendedTimeValue;
    int m_numSecondaryChunks;
    int m_nextChunkIndex;
    int m_headerSize;
    int m_chunkSize;
    int m_channel;
    QByteArray m_bytes;
    bool m_hasExtendedTimeValue;
}; // Codec

inline int CPCodec::nextChunkIndex() const noexcept
{
    return this->m_nextChunkIndex;
}

inline int CPCodec::channel() const noexcept
{
    return this->m_channel;
}

inline int CPCodec::headerSize() const noexcept
{
    return this->m_headerSize;
}

inline int CPCodec::chunkSize() const noexcept
{
    return this->m_chunkSize;
}

inline int CPCodec::numSecondaryChunks() const noexcept
{
    return this->m_numSecondaryChunks;
}

inline int CPCodec::size() const
{
    return this->m_bytes.size();
}

inline char *CPCodec::data()
{
    return this->m_bytes.data();
}

inline const char *CPCodec::constData() const
{
    return this->m_bytes.constData();
}

inline void CPCodec::resize(int size)
{
    this->m_bytes.resize(size);
}

inline void CPCodec::setChannel(int channel) noexcept
{
    this->m_channel = channel;
}

inline void CPCodec::setChunkSize(int chunkSize) noexcept
{
    this->m_chunkSize = chunkSize;
}

inline void CPCodec::setHeaderSize(int headerSize) noexcept
{
    this->m_headerSize = headerSize;
}

inline void CPCodec::setHasExtendedTimeValue(bool b) noexcept
{
    this->m_hasExtendedTimeValue = b;
}

inline bool CPCodec::hasExtendedTimeValue() const noexcept
{
    return this->m_hasExtendedTimeValue;
}

inline void CPCodec::clear()
{
    this->m_bytes.clear();
    this->m_numSecondaryChunks = -1;
    this->m_nextChunkIndex = -1;
    this->m_headerSize = 0;
}

inline void CPCodec::remove(int pos, int len)
{
    this->m_bytes.remove(pos, len);
}

inline void CPCodec::setExtendedTimeValue(quint32 extendedTimestamp) noexcept
{
    this->m_extendedTimeValue = extendedTimestamp;
}

inline quint32 CPCodec::extendedTimeValue() const noexcept
{
    return this->m_extendedTimeValue;
}

CPCodec::operator const QByteArray &() const noexcept
{
    return this->m_bytes;
}


#endif // CUTIEPOI_NET_CODEC_HPP
