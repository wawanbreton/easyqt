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
        QTimer::singleShot(
            _delay,
            this,
            [this, command]()
            {
                if (command->expectsAnswer())
                {
                    processCommand(command);
                }
                else
                {
                    processEvent(command);
                }
            });
        return true;
    }

    qCritical() << "Simulated commands queue does not support sending answers";
    return false;
}
