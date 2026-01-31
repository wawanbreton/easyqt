#include "communication/commands/command.h"

#include <QDebug>
#include <QTimer>

#include "easyqt/communication/commands/commandheader.h"


Command::Command(CommandHeader* header, QObject* parent)
    : QObject(parent)
    , _header(header)
{
    _header->setParent(this);
}

void Command::setTimeout(const int timeout)
{
    if (timeout > 0)
    {
        _timer = new QTimer(this);
        _timer->setSingleShot(true);
        _timer->setInterval(timeout);
        connect(_timer, &QTimer::timeout, this, &Command::error);
    }
}

std::optional<QByteArray> Command::streamData(CommandDataType::Enum dataType) const
{
    Q_UNUSED(dataType);

    return QByteArray();
}

bool Command::unstreamCommandData(const QByteArray& rawData, CommandDataType::Enum dataType)
{
    bool success = unstreamCommandDataImpl(rawData, dataType);

    if (success && dataType == CommandDataType::Answer)
    {
        emit answerReceived();
    }

    return success;
}

void Command::onSent()
{
    emit sent();

    if (_timer)
    {
        _timer->start();
    }
}

bool Command::unstreamCommandDataImpl(const QByteArray& rawData, CommandDataType::Enum dataType)
{
    Q_UNUSED(rawData);
    Q_UNUSED(dataType);

    return true;
}

QDebug operator<<(QDebug dbg, const Command* command)
{
    QDebugStateSaver saver(dbg);

    dbg.noquote().nospace() << "Command(" << command->_header << ", expectsAnswer:" << command->expectsAnswer() << ")";

    return dbg;
}
