#include "communication/commands/command.h"

#include <QDebug>
#include <QTimer>

#include "easyqt/communication/commands/commandheader.h"


Command::Command(CommandHeader::ConstPtr header, QObject* parent)
    : QObject(parent)
    , header_(header)
{
}

void Command::setTimeout(const int timeout)
{
    if (timeout > 0)
    {
        timer_ = new QTimer(this);
        timer_->setSingleShot(true);
        timer_->setInterval(timeout);
        connect(timer_, &QTimer::timeout, this, &Command::error);
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

    if (timer_)
    {
        timer_->start();
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

    dbg.noquote().nospace() << "Command(" << command->header_.get() << ", expectsAnswer:" << command->expectsAnswer()
                            << ")";

    return dbg;
}
