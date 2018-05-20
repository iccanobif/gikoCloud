#include "amf.hpp"
#include <math.h>

CPAmfVariantData::CPAmfVariantData(const CPAmfVariantData &other) :
    m_type(other.m_type)
{
    if (this->m_type == CPAmfType::String) {
        this->m_string = new QByteArray(*other.m_string);
    } else if (this->m_type == CPAmfType::Object) {
        this->m_object = new CPAmfObject(*other.m_object);
    } else if (this->m_type == CPAmfType::Number) {
        this->m_number = other.m_number;
    } else if (this->m_type == CPAmfType::Boolean) {
        this->m_boolean = other.m_boolean;
    }
}

void CPAmfVariantData::release() noexcept
{
    if (this->m_type == CPAmfType::String) {
        delete this->m_string;
    } else if (this->m_type == CPAmfType::Object) {
        delete this->m_object;
    }

    this->m_type = CPAmfType::None;
}

CPAmfVariantData::~CPAmfVariantData() noexcept
{
    this->release();
}

CPAmfVariant::CPAmfVariant()
{
    this->m_data = new CPAmfVariantData;
}

CPAmfVariant::CPAmfVariant(const CPAmfVariant &other) :
    m_data(other.m_data)
{
}

CPAmfVariant::CPAmfVariant(CPAmfVariant &&other) noexcept :
    m_data(std::move(other.m_data))
{
}

CPAmfVariant::CPAmfVariant(bool b)
{
    this->m_data = new CPAmfVariantData;
    this->setBoolean(b);
}

CPAmfVariant::CPAmfVariant(const CPAmfObject &object)
{
    this->m_data = new CPAmfVariantData;
    this->setObject(object);
}

CPAmfVariant::CPAmfVariant(const QByteArray &string)
{
    this->m_data = new CPAmfVariantData;
    this->setString(string);
}

CPAmfVariant::CPAmfVariant(double num)
{
    this->m_data = new CPAmfVariantData;
    this->setNumber(num);
}

CPAmfVariant::~CPAmfVariant() noexcept
{
}

CPAmfVariant &CPAmfVariant::operator=(const CPAmfVariant &other)
{
    CPAmfVariant tmp(other);
    *this = std::move(tmp);
    return *this;
}

CPAmfVariant &CPAmfVariant::operator=(CPAmfVariant &&other) noexcept
{
    if (&other != this) {
        this->m_data = std::move(other.m_data);
    }

    return *this;
}

void CPAmfVariant::setString(const QByteArray &string)
{
    if (this->type() != CPAmfType::String) {
        this->m_data->release();
        this->m_data->m_type = CPAmfType::String;
        this->m_data->m_string = new QByteArray();
    }

    *(this->m_data->m_string) = string ;
}

void CPAmfVariant::setObject(const CPAmfObject &object)
{
    if (this->type() != CPAmfType::Object) {
        this->m_data->release();
        this->m_data->m_type = CPAmfType::Object;
        this->m_data->m_object = new CPAmfObject();
    }

    *(this->m_data->m_object) = object;
}

void CPAmfVariant::setBoolean(bool b)
{
    if (this->type() != CPAmfType::Boolean) {
        this->m_data->release();
        this->m_data->m_type = CPAmfType::Boolean;
    }

    this->m_data->m_boolean = b;
}

void CPAmfVariant::setNull()
{
    if (this->type() != CPAmfType::Null) {
        this->m_data->release();
        this->m_data->m_type = CPAmfType::Null;
    }
}

void CPAmfVariant::setNumber(double num)
{
    if (this->type() != CPAmfType::Number) {
        this->m_data->release();
        this->m_data->m_type = CPAmfType::Number;
    }

    this->m_data->m_number = num;
}

void CPAmfVariant::setUndefined()
{
    if (this->type() != CPAmfType::Undefined) {
        this->m_data->release();
        this->m_data->m_type = CPAmfType::Undefined;
    }
}

CPAmfType CPAmfVariant::type() const
{
    return this->m_data->m_type;
}

bool CPAmfVariant::boolean(bool *ok) const
{
    if (this->type() != CPAmfType::Boolean) {
        if (ok != nullptr) {
            *ok = false;
        }
        return false;
    }

    if (ok != nullptr) {
        *ok = true;
    }
    return this->m_data->m_boolean;
}

double CPAmfVariant::number(bool *ok) const
{
    if (this->type() != CPAmfType::Number) {
        if (ok != nullptr) {
            *ok = false;
        }
        return NAN;
    }

    if (ok != nullptr) {
        *ok = true;
    }
    return this->m_data->m_number;
}

CPAmfObject *CPAmfVariant::object()
{
    if (this->type() == CPAmfType::Object) {
        return this->m_data->m_object;
    }

    return nullptr;
}

const CPAmfObject *CPAmfVariant::constObject() const
{
    if (this->type() == CPAmfType::Object) {
        return this->m_data->m_object;
    }

    return nullptr;
}

QByteArray *CPAmfVariant::string()
{
    if (this->type() == CPAmfType::String) {
        return this->m_data->m_string;
    }

    return nullptr;
}

const QByteArray *CPAmfVariant::constString() const
{
    if (this->type() == CPAmfType::String) {
        return this->m_data->m_string;
    }

    return nullptr;
}

void CPAmfVariant::setType(CPAmfType type)
{
    this->m_data->release();
    this->m_data->m_type = type;

    if (type == CPAmfType::Object) {
        this->m_data->m_object = new CPAmfObject();
    } else if (type == CPAmfType::String) {
        this->m_data->m_string = new QByteArray();
    }
}


CPAmfProperty::CPAmfProperty() :
    CPAmfVariant(),
    m_name()
{
}

CPAmfProperty::CPAmfProperty(const CPAmfProperty &other) :
    CPAmfVariant(other),
    m_name(other.m_name)
{
}

CPAmfProperty::CPAmfProperty(CPAmfProperty &&other) noexcept :
    CPAmfVariant(std::move(other)),
    m_name(std::move(other.m_name))
{
}

CPAmfProperty::CPAmfProperty(const QByteArray &name, bool b) :
    CPAmfVariant(b),
    m_name(name)
{
}

CPAmfProperty::CPAmfProperty(const QByteArray &name, const CPAmfObject &obj) :
    CPAmfVariant(obj),
    m_name(name)
{
}

CPAmfProperty::CPAmfProperty(const QByteArray &name, const CPAmfVariant &data) :
    CPAmfVariant(data),
    m_name(name)
{
}

CPAmfProperty::CPAmfProperty(const QByteArray &name, const QByteArray &str) :
    CPAmfVariant(str),
    m_name(name)
{
}

CPAmfProperty::CPAmfProperty(const QByteArray &name, double num) :
    CPAmfVariant(num),
    m_name(name)
{
}

CPAmfProperty::CPAmfProperty(const QByteArray &name, CPAmfType type) :
    CPAmfVariant(),
    m_name(name)
{
    this->setType(type);
}

CPAmfProperty::~CPAmfProperty() noexcept
{
}

CPAmfProperty &CPAmfProperty::operator=(CPAmfProperty &&other) noexcept
{
    if (&other != this) {
        this->m_name = std::move(other.m_name);
        this->m_data = std::move(other.m_data);
    }

    return *this;
}
