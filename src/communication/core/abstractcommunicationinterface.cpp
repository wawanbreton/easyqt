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

void AbstractCommunicationInterface::sendRequest(
    const quint32& id,
    const QList<QVariant>& dataRequest,
    QObject* receiver,
    const SlotAnswerType& slotAnswer,
    const SlotNoArgType& slotError,
    const SlotNoArgType& slotSent,
    const int& timeout)
{
    return sendCommandImpl(id, dataRequest, receiver, slotAnswer, slotError, slotSent, timeout > 0 ? timeout : 0, true);
}

void AbstractCommunicationInterface::sendEvent(
    const quint32& id,
    const QList<QVariant>& dataRequest,
    QObject* receiver,
    const SlotNoArgType& slotError,
    const SlotNoArgType& slotSent)
{
    return sendCommandImpl(id, dataRequest, receiver, nullptr, slotError, slotSent, -1, false);
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

void AbstractCommunicationInterface::sendCommandImpl(
    const quint32& id,
    const QList<QVariant>& dataRequest,
    QObject* receiver,
    const SlotAnswerType& slotAnswer,
    const SlotNoArgType& slotError,
    const SlotNoArgType& slotSent,
    const int& timeout,
    const bool& expectsAnswer)
{
    Command* command = _queue->makeCommand(id, timeout);
    if (command)
    {
        command->setData(CommandDataType::Request, dataRequest);
        command->setExpectsAnswer(expectsAnswer);

        if (receiver)
        {
            if (slotAnswer)
            {
                connect(command, &Command::answerReceived, receiver, slotAnswer);
            }
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
    else
    {
        qWarning() << "Unable to build command with id" << command;
        if (slotError)
        {
            QTimer::singleShot(0, receiver, slotError);
        }
    }
}
