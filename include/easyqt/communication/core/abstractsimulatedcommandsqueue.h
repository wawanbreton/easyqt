#pragma once

#include "communication/core/abstractcommandsqueue.h"

/*! @brief Very simple implementation of a commands queue, to handle simulated commands which
 *         actually just process a local method call
 *
 *  Child classes should implement the processCommand method and set the proper answer data directly
 *  to the command */
class AbstractSimulatedCommandsQueue : public AbstractCommandsQueue
{
    Q_OBJECT

public:
    explicit AbstractSimulatedCommandsQueue(QObject* parent, const std::chrono::milliseconds& delay);

    virtual bool sendCommandImpl(Command* command, CommandDataType::Enum dataType) override final;

protected:
    virtual void processCommand(Command* command) = 0;

    virtual void processEvent(quint32 id, const QList<QVariant>& requestData) = 0;

private:
    const std::chrono::milliseconds _delay;
};
