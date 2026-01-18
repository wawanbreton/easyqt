#include "communication/core/abstractcommanddatastreamer.h"

#include <QDebug>

#include "bitfield.h"


AbstractCommandDataStreamer::AbstractCommandDataStreamer(QObject* parent)
    : QObject(parent)
{
}

QList<QVariant>
    AbstractCommandDataStreamer::unstreamInnerDataReceiveSuccess(const quint32& commandId, const QByteArray& data)
{
    // Expects a single byte containing 1 for success or anything else for failure

    quint8 returnCode;
    bool ok = BitField::toUInt8(data, returnCode) && returnCode == 1;

    if (! ok)
    {
        qWarning() << "Received KO code for command" << BitField::toHex(commandId, '\0');
    }

    return { ok };
}

QList<QVariant>
    AbstractCommandDataStreamer::unstreamInnerDataReceiveOkKo(const quint32& commandId, const QByteArray& data)
{
    // Expects two bytes containing ASCII "OK" for success or ASCII "KO" for failure

    bool ok = false;
    quint16 returnCode;
    ok = BitField::toUInt16(data, returnCode) && returnCode == 0x4f4b;

    if (! ok)
    {
        qWarning() << "Received KO code for command" << BitField::toHex(commandId, '\0');
    }

    return { ok };
}

QByteArray AbstractCommandDataStreamer::streamInnerDataSendOkKo(const bool& success)
{
    return BitField::toByteArray(quint16(success ? 0x4f4b : 0x4b4f));
}

#warning Optimize by giving arrays as input reference
QByteArray AbstractCommandDataStreamer::writeString(const QString& string)
{
    return writeRawData(string.toUtf8());
}

QByteArray AbstractCommandDataStreamer::writeRawData(const QByteArray& rawData)
{
    QByteArray sizedData;

    sizedData.append(BitField::toByteArray(static_cast<quint16>(rawData.size())));
    sizedData.append(rawData);

    return sizedData;
}

bool AbstractCommandDataStreamer::parseString(const QByteArray& data, QString& string, quint16* consumedBytes)
{
    QByteArray rawStringData;
    if (parseRawData(data, rawStringData, consumedBytes))
    {
        string = QString::fromUtf8(rawStringData);
        return true;
    }

    return false;
}

bool AbstractCommandDataStreamer::parseRawData(
    const QByteArray& messageData,
    QByteArray& innerData,
    quint16* consumedBytes)
{
    quint16 dataSize;
    if (BitField::toUInt16(messageData, dataSize))
    {
        if (messageData.size() >= (dataSize + 2))
        {
            innerData = messageData.mid(2, dataSize);
            if (consumedBytes)
            {
                *consumedBytes = dataSize + 2;
            }
            return true;
        }
    }

    return false;
}

QByteArray AbstractCommandDataStreamer::writeSuccess(bool success)
{
    return BitField::toByteArray(static_cast<quint8>(success ? 1 : 0));
}
