#pragma once

#include <QByteArray>
#include <QObject>
#include <QVariant>

#include "easyqt/communication/core/commanddatatype.h"

class CommandHeader;

/*! @brief Abstract class used to serialize/unserialize inner data of commands send through a communication tunnel */
class AbstractCommandDataStreamer : public QObject
{
public:
    explicit AbstractCommandDataStreamer(QObject* parent = nullptr);

    virtual QByteArray
        streamCommandData(const quint32& commandId, const QList<QVariant>& userData, CommandDataType::Enum dataType)
            const
        = 0;

    virtual QList<QVariant> unstreamCommandData(
        const quint32& commandId,
        const QByteArray& rawData,
        CommandDataType::Enum dataType,
        const QList<QVariant>& dataSend) const
        = 0;

protected:
    static QByteArray writeString(const QString& string);

    static QByteArray writeRawData(const QByteArray& rawData);

    static bool parseString(const QByteArray& data, QString& string, quint16* consumedBytes = nullptr);

    static bool parseRawData(const QByteArray& messageData, QByteArray& innerData, quint16* consumedBytes = nullptr);

    static QByteArray writeSuccess(bool success);

    static QList<QVariant> unstreamInnerDataReceiveSuccess(const quint32& commandId, const QByteArray& data);

    static QList<QVariant> unstreamInnerDataReceiveOkKo(const quint32& commandId, const QByteArray& data);

    static QByteArray streamInnerDataSendOkKo(const bool& success);
};
