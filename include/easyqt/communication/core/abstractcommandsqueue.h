#pragma once

#include <QObject>
#include <QQueue>

#include "easyqt/communication/commands/commandheader.h"
#include "easyqt/communication/core/commanddatatype.h"

class Command;

/*! @brief Abstract class to manage a commands queue.
 *
 *  Command sending :
 *   * If commands can be sent in parallel, give parallelCommands=true at constructor, then each
 *     time a new command is appended, it will be send immediatly
 *   * If commands have to be sent sequentially, give parallelCommands=false at constructor, then
 *     when a command is appended, it will be sent only if the previous command has been answered
 *     or canceled
 *   * Override the sendCommandImpl method to actually process the command sending (using a
 *     QIODevice, a local method call, a pigeon, ...)
 *   * Override the makeNewHeader method if the handled commands use a specific header
 *
 *  Command receiving : it is up to each child implementation to make sure that the command answer
 *                      data is set somehow */
class AbstractCommandsQueue : public QObject
{
    Q_OBJECT

public:
    explicit AbstractCommandsQueue(bool parallelCommands, int defaultTimeout, QObject* parent = nullptr);

    virtual ~AbstractCommandsQueue();

    /*! @brief Makes a new command to be sent
     *  @param id The command ID
     *  @param timeout The command answering timeout : <0 for no timeout,
     *                                                 =0 for default timeout
     *                                                 >0 for a specific timeout */
    Command* makeCommand(const quint32 id, const int timeout);

    void append(Command* command);

    bool hasPendingCommands() const
    {
        return ! _commands.isEmpty();
    }

    void cancelCurrentCommand();

    bool handleParallelCommands() const
    {
        return _parallelCommands;
    }

signals:
    void queueChanged(const bool& queueEmpty);

    void commandReceived(Command* command);

    void commandError();

protected:
    virtual Command* makeCommandImpl(CommandHeader* header);

    virtual CommandHeader* makeHeader(const quint32 commandId) const = 0;

    virtual bool sendCommandImpl(Command* command, CommandDataType::Enum dataType) = 0;

    const QQueue<Command*>& getCommands() const
    {
        return _commands;
    }

    void onCommandReceived(Command* command);

private:
    void sendRequest(Command* command);

    void sendAnswer(Command* command);

    void sendCommand(Command* command);

    void onCommandError(Command* command);

    void onCommandTreated(Command* command);

private:
    const bool _parallelCommands;
    const int _defaultTimeout;
    QQueue<Command*> _commands;
};
