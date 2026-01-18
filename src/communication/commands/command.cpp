#include "communication/commands/command.h"

#include <QDebug>
#include <QTimer>

#include "bitfield.h"


Command::Command(const CommandHeader* header, int timeout, QObject* parent)
    : QObject(parent)
    , _header(header)
{
    if (timeout > 0)
    {
        _timer = new QTimer(this);
        _timer->setSingleShot(true);
        _timer->setInterval(timeout);
        connect(_timer, &QTimer::timeout, this, &Command::error);
    }
}

Command::~Command()
{
    delete _header;
}

QList<QVariant> Command::getData(const CommandDataType::Enum& dataType, bool* dataPresent) const
{
    auto iterator = _data.find(dataType);
    if (iterator != _data.end())
    {
        if (dataPresent)
        {
            *dataPresent = true;
        }
        return iterator.value();
    }
    else
    {
        if (dataPresent)
        {
            *dataPresent = false;
        }
        qCritical() << "Asked for data" << dataType << "but it has not been set yet";
        return QList<QVariant>();
    }
}

void Command::setData(const CommandDataType::Enum dataType, const QList<QVariant>& data)
{
    if (! _data.contains(dataType))
    {
        _data.insert(dataType, data);

        if (dataType == CommandDataType::Answer)
        {
            emit answerReceived(data, getData(CommandDataType::Request));
        }
    }
    else
    {
        qCritical() << "Unable to set data twice" << dataType;
    }
}

void Command::onSent()
{
    emit sent();

    if (_timer)
    {
        _timer->start();
    }
}

QDebug operator<<(QDebug dbg, const Command* command)
{
    QDebugStateSaver saver(dbg);

    QString fullString("Command(ID:%1, expectsAnswer:%2, %3)");
    fullString = fullString.arg(BitField::toHex(command->getId(), '\0'));
    fullString = fullString.arg(command->expectsAnswer());

    QStringList dataStr;
    for (auto iterator = command->_data.begin(); iterator != command->_data.end(); ++iterator)
    {
        QStringList dataGroupStr;

        for (const QVariant& value : iterator.value())
        {
            if (value.typeId() == qMetaTypeId<QByteArray>())
            {
                const QByteArray data = value.toByteArray();
                QString valueStr("%1b:%2");
                valueStr = valueStr.arg(data.size());
                valueStr = valueStr.arg(BitField::toHex(value.toByteArray(), '\0', 32));
                dataGroupStr << valueStr;
            }
            else
            {
                QString tmpString;
                QDebug tmpDebug(&tmpString);
                tmpDebug << value;
                dataGroupStr << tmpString;
            }
        }

        QString dataGroupFullStr("%1(%2)");
        dataGroupFullStr = dataGroupFullStr.arg(CommandDataType::toString(iterator.key()));
        dataGroupFullStr = dataGroupFullStr.arg(dataGroupStr.join(", "));
        dataStr << dataGroupFullStr;
    }

    fullString = fullString.arg(dataStr.join(", "));
    dbg.noquote() << fullString;

    return dbg;
}
