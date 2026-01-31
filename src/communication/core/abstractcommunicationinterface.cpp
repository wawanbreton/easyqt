#include "communication/core/abstractcommunicationinterface.h"

#include <QDebug>

#include "communication/commands/command.h"
#include "communication/core/abstractcommandsqueue.h"
#include "communication/core/abstractdevicecommandsqueue.h"


AbstractCommunicationInterface::AbstractCommunicationInterface(AbstractCommandsQueue* queue, QObject* parent)
    : QObject(parent)
    , _queue(queue)
{
    _queue->setParent(this);
    connect(_queue, &AbstractCommandsQueue::commandReceived, this, &AbstractCommunicationInterface::onCommandReceived);
}

QIODevice* AbstractCommunicationInterface::accessDevice()
{
    auto* deviceCommandsQueue = qobject_cast<AbstractDeviceCommandsQueue*>(_queue);
    if (deviceCommandsQueue)
    {
        return deviceCommandsQueue->accessDevice();
    }

    return nullptr;
}

const QIODevice* AbstractCommunicationInterface::getDevice() const
{
    const auto* deviceCommandsQueue = qobject_cast<const AbstractDeviceCommandsQueue*>(_queue);
    if (deviceCommandsQueue)
    {
        return deviceCommandsQueue->getDevice();
    }

    return nullptr;
}

void AbstractCommunicationInterface::cancelCurrentCommand()
{
    _queue->cancelCurrentCommand();
}

Command* AbstractCommunicationInterface::makeRequest(const quint32 id, const int timeout)
{
    return _queue->makeCommand(id, timeout);
}

Command* AbstractCommunicationInterface::makeEvent(const quint32 id)
{
    return _queue->makeCommand(id, -1);
}

void AbstractCommunicationInterface::sendRequest(
    Command* command,
    QObject* receiver,
    const SlotNoArgType& slotError,
    const SlotNoArgType& slotSent)
{
    return sendCommand(command, receiver, slotError, slotSent, true);
}

void AbstractCommunicationInterface::sendRequestEmptyAnswer(
    Command* command,
    QObject* receiver,
    const SlotNoArgType& slotAnswer,
    const SlotNoArgType& slotError,
    const SlotNoArgType& slotSent)
{
    if (receiver && slotAnswer)
    {
        connect(command, &Command::answerReceived, receiver, slotAnswer);
    }

    sendRequest(command, receiver, slotError, slotSent);
}

void AbstractCommunicationInterface::sendEvent(
    Command* command,
    QObject* receiver,
    const SlotNoArgType& slotError,
    const SlotNoArgType& slotSent)
{
    return sendCommand(command, receiver, slotError, slotSent, false);
}

bool AbstractCommunicationInterface::onCommandReceivedImpl(Command* command)
{
    Q_UNUSED(command)
    return false;
}

void AbstractCommunicationInterface::onCommandReceived(Command* command)
{
    command->setExpectsAnswer(onCommandReceivedImpl(command));
}

void AbstractCommunicationInterface::sendCommand(
    Command* command,
    QObject* receiver,
    const SlotNoArgType& slotError,
    const SlotNoArgType& slotSent,
    const bool expectsAnswer)
{
    command->setExpectsAnswer(expectsAnswer);

    if (receiver)
    {
        if (slotSent)
        {
            connect(command, &Command::sent, receiver, slotSent);
        }
        if (slotError)
        {
            connect(command, &Command::error, receiver, slotError);
        }
    }

    _queue->append(command);
}
