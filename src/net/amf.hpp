#ifndef CUTIEPOI_NET_AMF_HPP
#define CUTIEPOI_NET_AMF_HPP

#include <QSharedDataPointer>
#include <QVector>

class CPAmfProperty;
class CPAmfVariantData;
typedef QVector<CPAmfProperty> CPAmfObject;

enum class CPAmfType : quint8 {
    Number = 0x00,
    Boolean = 0x01,
    String = 0x02,
    Object = 0x03,
    MovieClip = 0x04,
    Null = 0x05,
    Undefined = 0x06,
    Reference = 0x07,
    MixedArray = 0x08,
    EndOfObject = 0x09,
    Array = 0x0a,
    Date = 0x0b,
    LongString = 0x0c,
    Unsupported = 0x0d,
    RecordSet = 0x0e,
    Xml = 0x0f,
    Class = 0x10,
    Amf3Object = 0x11,
    None = 0xff
};

/**
 * Class representing an Amf value. Can only hold one type of value at a given
 * time. The value is implicitly shared.
 */
class CPAmfVariant {
public:
    /**
     * Construct an AmfVariant object.
     */
    CPAmfVariant();

    /**
     * Copy construct an AmfVariant object.
     * @param[in] other The variant to copy.
     */
    CPAmfVariant(const CPAmfVariant &other);
    /**
     * Move construct a variant.
     * @param[in,out] other The variant to move.
     */
    CPAmfVariant(CPAmfVariant &&other) noexcept;
    /**
     * Construct a variant holding a string value.
     * @param[in] string The string value to hold.
     */
    CPAmfVariant(const QByteArray &string);
    /**
     * Construct a variant holding an object value.
     * @param[in] object The object value to hold.
     */
    CPAmfVariant(const CPAmfObject &object);
    /**
     * Construct a variant holding a number value.
     * @param[in] num The number value to hold.
     */
    CPAmfVariant(double num);
    /**
     * Construct a variant holding a boolean value.
     * @param[in] b The boolean value to hold.
     */
    CPAmfVariant(bool b);

    /**
     * Copy assign a variant.
     * @param[in] other The variant to copy.
     */
    CPAmfVariant &operator=(const CPAmfVariant &other);
    /**
     * Move assign a variant.
     * @param[in,out] other The variant to move.
     */
    CPAmfVariant &operator=(CPAmfVariant &&other) noexcept;

    /**
     * Destroy a variant.
     */
    virtual ~CPAmfVariant() noexcept;

    /**
     * Change the type of the variant to AmfType::String.
     * @param[in] string The string data to hold. A copy of the string is created.
     */
    void setString(const QByteArray &string);
    /**
     * Change the type of the variant to AmfType::Object.
     * @param[in] object The Object data to hold. A copy of the object is created.
     */
    void setObject(const CPAmfObject &object);
    /**
     * Change the type of the variant to AmfType::Number.
     * @param[in] num The number data.
     */
    void setNumber(double num);
    /**
     * Change the type of the variant to AmfType::Boolean.
     * @param[in] boolean The boolean data.
     */
    void setBoolean(bool boolean);
    /**
     * Change the type of the variant to AmfType::Null.
     */
    void setNull();
    /**
     * Change the type of the variant to AmfType::Undefined.
     */
    void setUndefined();
    /**
     * Change the type of the variant.
     * @param[in] type The new type.
     */
    void setType(CPAmfType type);

    /**
     * @return the internal type of variant.
     */
    CPAmfType type() const;
    /**
     * Get the AmfType::Number value.
     * @param[out] ok If not nullptr: *ok is set to false if data type is not
     * AmfType::Number; otherwise *ok is set to true.
     * @return the number value. NAN may indicate an error.
     */
    double number(bool *ok = nullptr) const;
    /**
     * Get the AmfType::Boolean value.
     * @param[out] ok If not nullptr: *ok is set to false if data type is not
     * AmfType::Boolean; otherwise *ok is set to true.
     * @return The boolean value. false may indicate an error.
     */
    bool boolean(bool *ok = nullptr) const;
    /**
     * Get the object data.
     * @return the object data if type() is AmfType::Object; otherwise nullptr.
     */
    CPAmfObject *object();
    /**
     * Get the object data.
     * @return the object data if type() is AmfType::Object; otherwise nullptr.
     */
    const CPAmfObject *constObject() const;
    /**
     * Get the string data.
     * @return the string data if type() is AmfType::String; otherwise nullptr.
     */
    QByteArray *string();
    /**
     * Get the string data.
     * @return the string data if type() is AmfType::String; otherwise nullptr.
     */
    const QByteArray *constString() const;

protected:
    QSharedDataPointer<CPAmfVariantData> m_data;
};

class CPAmfProperty : public CPAmfVariant {
public:
    /**
     * Construct an AmfProperty.
     */
    CPAmfProperty();
    /**
     * Copy construct an AmfProperty.
     * @param[in] other The object to copy.
     */
    CPAmfProperty(const CPAmfProperty &other);
    /**
     * Move construct an AmfProperty.
     * @param[in,out] other The property to move.
     */
    CPAmfProperty(CPAmfProperty &&other) noexcept;
    /**
     * Create a property with name and type set.
     *
     * This constructor is usefull for creating properties of None Value types,
     * and properties containing zero values.
     *
     * @param[in] name The name of the property.
     * @param[in] type The property type.
     */
    CPAmfProperty(const QByteArray &name, CPAmfType type);
    /**
     * Create a named string property.
     * @param[in] name The name of the property.
     * @param[in] str The string value.
     */
    CPAmfProperty(const QByteArray &name, const QByteArray &str);
    /**
     * Create a named object property.
     * @param[in] name The name of the property.
     * @param[in] obj The object value.
     */
    CPAmfProperty(const QByteArray &name, const CPAmfObject &obj);
    /**
     * Create a named number property.
     * @param[in] name The name of the property.
     * @param[in] num The number value.
     */
    CPAmfProperty(const QByteArray &name, double num);
    /**
     * Create a named boolean property.
     * @param[in] name The name of the property.
     * @param[in] b The object value.
     */
    CPAmfProperty(const QByteArray &name, bool b);
    /**
     * Create a named property, with the type and value set the same as src.
     * @param[in] name The name of the property.
     * @param[in] src The value to hold.
     */
    CPAmfProperty(const QByteArray &name, const CPAmfVariant &src);
    /**
     * Destroy an AmfProperty.
     */
    virtual ~CPAmfProperty() noexcept;

    /**
     * Copy assign an AmfProperty.
     * @param[in] other The property to copy.
     */
    inline CPAmfProperty &operator=(const CPAmfProperty &other);
    /**
     * Move assign an AmfProperty.
     * @param[in,out] other The property to move.
     */
    CPAmfProperty &operator=(CPAmfProperty &&other) noexcept;

    /**
     * Set the property name.
     * @param[in] name The new property name.
     */
    inline void setName(const QByteArray &name);
    /**
     * @return the property name.
     */
    inline const QByteArray &constName() const;
    /**
     * @return the property name.
     */
    inline QByteArray &name();
protected:
    QByteArray m_name;
};

class CPAmfVariantData : public QSharedData {
public:
    inline CPAmfVariantData() noexcept;
    CPAmfVariantData(const CPAmfVariantData &other);
    virtual ~CPAmfVariantData() noexcept;

    CPAmfType m_type;

    union {
        QByteArray *m_string;
        CPAmfObject *m_object;
        bool m_boolean;
        double m_number;
    };

    /**
     * Release any memory allocated by this object.
     */
    void release() noexcept;
};

inline CPAmfVariantData::CPAmfVariantData() noexcept : m_type(CPAmfType::None) {}

inline CPAmfProperty &CPAmfProperty::operator=(const CPAmfProperty &other)
{
    CPAmfProperty tmp(other);
    *this = std::move(tmp);
    return *this;
}

inline void CPAmfProperty::setName(const QByteArray &name)
{
    this->m_name = name;
}

inline QByteArray &CPAmfProperty::name()
{
    return this->m_name;
}

inline const QByteArray &CPAmfProperty::constName() const
{
    return this->m_name;
}

#endif // CUTIEPOI_NET_AMF_HPP
