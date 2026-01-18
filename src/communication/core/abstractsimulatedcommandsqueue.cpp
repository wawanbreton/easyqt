#include "communication/core/abstractsimulatedcommandsqueue.h"

#include <QDebug>
#include <QTimer>

#include "communication/commands/command.h"


AbstractSimulatedCommandsQueue::AbstractSimulatedCommandsQueue(QObject* parent, const std::chrono::milliseconds& delay)
    : AbstractCommandsQueue(false, 1000, parent)
    , _delay(delay)
{
}

bool AbstractSimulatedCommandsQueue::sendCommandImpl(Command* command, CommandDataType::Enum dataType)
{
    if (dataType == CommandDataType::Request)
    {
        if (command->expectsAnswer())
        {
            QTimer::singleShot(
                _delay,
                this,
                [this, command]()
                {
                    processCommand(command);
                });
        }
        else
        {
            const quint32 id = command->getId();
            const QList<QVariant> requestData = command->getRequestData();
            QTimer::singleShot(
                _delay,
                this,
                [this, id, requestData]()
                {
                    processEvent(id, requestData);
                });
        }
        return true;
    }

    qCritical() << "Simulated commands queue does not support sending answers";
    return false;
}
