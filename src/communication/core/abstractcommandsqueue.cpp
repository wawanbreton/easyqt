#include "communication/core/abstractcommandsqueue.h"

#include <QDebug>

#include "communication/commands/command.h"
#include "communication/commands/idbasedheader.h"

// #define DISPLAY_COMMANDS 1


AbstractCommandsQueue::AbstractCommandsQueue(bool parallelCommands, int defaultTimeout, QObject* parent)
    : QObject(parent)
    , _parallelCommands(parallelCommands)
    , _defaultTimeout(defaultTimeout)
{
}

AbstractCommandsQueue::~AbstractCommandsQueue()
{
    for (auto command : _commands)
    {
        disconnect(command, nullptr, this, nullptr);
        command->manualFail();
    }
}

Command* AbstractCommandsQueue::makeCommand(const quint32 id, const int timeout)
{
    Command* command = makeCommandImpl(makeHeader(id));
    if (timeout >= 0)
    {
        command->setTimeout(timeout == 0 ? _defaultTimeout : timeout);
    }
    return command;
}

void AbstractCommandsQueue::append(Command* command)
{
    _commands.enqueue(command);
    emit queueChanged(false);

#ifdef DISPLAY_COMMANDS
    qDebug() << "Command enqueued" << command << _commands.size();
#endif

    connect(
        command,
        &Command::answerReceived,
        this,
        [this, command]()
        {
            onCommandTreated(command);
        });
    connect(command, &Command::error, this, &AbstractCommandsQueue::commandError);
    connect(
        command,
        &Command::error,
        this,
        [this, command]()
        {
            onCommandError(command);
        });

    if (_parallelCommands || _commands.count() == 1)
    {
        sendRequest(command);
    }
}

void AbstractCommandsQueue::cancelCurrentCommand()
{
    if (! _commands.isEmpty())
    {
        _commands.head()->manualFail();
    }
}

Command* AbstractCommandsQueue::makeCommandImpl(CommandHeader* header)
{
    return new Command(header, this);
}

void AbstractCommandsQueue::onCommandReceived(Command* command)
{
#ifdef DISPLAY_COMMANDS
    qDebug() << "Command received" << command;
#endif

    connect(
        command,
        &Command::answerReceived,
        this,
        [this, command]
        {
            sendAnswer(command);
        });

    emit commandReceived(command);

    if (command->expectsAnswer())
    {
        // The command expects a later answer : delete it when it has been set
        connect(command, &Command::answerReceived, command, &Command::deleteLater);
    }
    else
    {
        // The command does not expect an answer : delete it now
        command->deleteLater();
    }
}

void AbstractCommandsQueue::sendRequest(Command* command)
{
#ifdef DISPLAY_COMMANDS
    qDebug() << "Sending command" << command;
#endif

    if (sendCommandImpl(command, CommandDataType::Request))
    {
        command->onSent();
    }
    else
    {
        command->manualFail();
    }

    if (! command->expectsAnswer())
    {
        onCommandTreated(command);
    }
}

void AbstractCommandsQueue::sendAnswer(Command* command)
{
#ifdef DISPLAY_COMMANDS
    qDebug() << "Sending answer for command" << command;
#endif

    sendCommandImpl(command, CommandDataType::Answer);
}

void AbstractCommandsQueue::onCommandError(Command* command)
{
    qWarning() << "Error for command" << command;
    onCommandTreated(command);
}

void AbstractCommandsQueue::onCommandTreated(Command* command)
{
#ifdef DISPLAY_COMMANDS
    qDebug() << "Command treated" << command << _commands.size();
#endif

    if (_commands.removeAll(command))
    {
        command->deleteLater();
        emit queueChanged(_commands.isEmpty());

        if (! _parallelCommands && ! _commands.isEmpty())
        {
            sendRequest(_commands.head());
        }
    }
    else
    {
        qCritical() << "Command command is not registered";
    }
}
