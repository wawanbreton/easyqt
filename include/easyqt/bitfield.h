#pragma once

#include <QByteArray>

namespace BitField
{    
    template <class Type>
    QByteArray toByteArray(const Type &value, bool invertBytes = false);

    QByteArray toByteArray(const quint8 *value, quint8 size, bool invertBytes = false);

    bool toUInt8(const QByteArray &data, quint8 &value);

    bool toUInt16(const QByteArray &data, quint16 &value, bool invertBytes = false);

    bool toInt16(const QByteArray &data, qint16 &value, bool invertBytes = false);

    bool toUInt32(const QByteArray &data, quint32 &value, bool invertBytes = false);

    bool toInt32(const QByteArray &data, qint32 &value, bool invertBytes = false);

    bool toFloat(const QByteArray &data, float &value, bool invertBytes = false);

    bool toValue(const QByteArray &data, quint8 *dst, quint8 size, bool invertBytes);

    template <class Type>
    bool toValue(const QByteArray &data, Type &value, bool invertBytes = false);

    QString toHex(const QByteArray &data, char separator = ':', int maxSize = -1);

    QString toHex(const quint32 &value, char separator = ':');

    QString toHex(const quint16 &value, char separator = ':');

    QString toHex(const quint8 &value, char separator = ':');

    QByteArray reverse(const QByteArray &data);

    template <typename T>
    void setBit(T &value, quint8 bit)
    { value |= (T(1) << bit); }

    static inline void unsetBit(quint64 &value, quint8 bit)
    { value &= ~(quint64(1) << bit); }
}

template <class Type>
QByteArray BitField::toByteArray(const Type &value, bool invertBytes)
{
    return toByteArray(reinterpret_cast<const quint8 *>(&value), sizeof(Type), invertBytes);
}

template <class Type>
bool BitField::toValue(const QByteArray &data, Type &value, bool invertBytes)
{
    return toValue(data, reinterpret_cast<quint8 *>(&value), sizeof(Type), invertBytes);
}
