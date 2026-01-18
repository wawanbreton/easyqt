#include "bitfield.h"

#include <QDebug>


QByteArray BitField::toByteArray(const quint8* value, quint8 size, bool invertBytes)
{
    QByteArray data;

    int start = invertBytes ? 0 : size - 1;
    int end = invertBytes ? size : -1;
    int delta = invertBytes ? 1 : -1;

    for (int i = start; i != end; i += delta)
    {
        data.append(reinterpret_cast<const char*>(value) + i, 1);
    }

    return data;
}

bool BitField::toUInt8(const QByteArray& data, quint8& value)
{
    return toValue(data, &value, 1, false);
}

bool BitField::toUInt16(const QByteArray& data, quint16& value, bool invertBytes)
{
    return toValue(data, reinterpret_cast<quint8*>(&value), 2, invertBytes);
}

bool BitField::toInt16(const QByteArray& data, qint16& value, bool invertBytes)
{
    return toValue(data, reinterpret_cast<quint8*>(&value), 2, invertBytes);
}

bool BitField::toUInt32(const QByteArray& data, quint32& value, bool invertBytes)
{
    return toValue(data, reinterpret_cast<quint8*>(&value), 4, invertBytes);
}

bool BitField::toInt32(const QByteArray& data, qint32& value, bool invertBytes)
{
    return toValue(data, reinterpret_cast<quint8*>(&value), 4, invertBytes);
}

bool BitField::toFloat(const QByteArray& data, float& value, bool invertBytes)
{
    return toValue(data, reinterpret_cast<quint8*>(&value), 4, invertBytes);
}

bool BitField::toValue(const QByteArray& data, quint8* dst, quint8 size, bool invertBytes)
{
    if (data.size() >= size)
    {
        int start = invertBytes ? 0 : size - 1;
        int end = invertBytes ? size : -1;
        int delta = invertBytes ? 1 : -1;
        int dstDelta = 0;

        for (int i = start; i != end; i += delta)
        {
            dst[dstDelta++] = data[i];
        }

        return true;
    }
    else
    {
        qWarning() << "Not enough data to read";
    }

    return false;
}

QString BitField::toHex(const QByteArray& data, char separator, int maxSize)
{
    QByteArray toHex = data.mid(0, maxSize).toHex(separator);

    if (maxSize > 0 && data.size() > maxSize)
    {
        toHex.append("...");
    }

    return QString("0x%1").arg(QString::fromUtf8(toHex));
}

QString BitField::toHex(const quint32& value, char separator)
{
    return toHex(toByteArray(value), separator);
}

QString BitField::toHex(const quint16& value, char separator)
{
    return toHex(toByteArray(value), separator);
}

QString BitField::toHex(const quint8& value, char separator)
{
    return toHex(toByteArray(value), separator);
}

QByteArray BitField::reverse(const QByteArray& data)
{
    QByteArray result(data.size(), '\0');

    for (int i = 0; i < data.size(); ++i)
    {
        result[data.size() - 1 - i] = data[i];
    }

    return result;
}
