#include "communication/core/abstractdevicecommandsqueue.h"

#include <QDebug>

#include "bitfield.h"
#include "communication/commands/command.h"


AbstractDeviceCommandsQueue::AbstractDeviceCommandsQueue(
    QIODevice* device,
    bool parallelCommands,
    int defaultTimeout,
    QObject* parent,
    const bool& logRawData)
    : AbstractCommandsQueue(parallelCommands, defaultTimeout, parent)
    , _device(device)
    , _logRawData(logRawData)
{
    connect(_device, &QIODevice::readyRead, this, &AbstractDeviceCommandsQueue::onReadyRead);
    connect(_device, &QIODevice::bytesWritten, this, &AbstractDeviceCommandsQueue::onDeviceBytesWritten);
}

bool AbstractDeviceCommandsQueue::sendCommandImpl(Command* command, CommandDataType::Enum dataType)
{
    std::optional<QByteArray> commandRawData = command->streamData(dataType);
    if (! commandRawData.has_value())
    {
        qWarning() << "Could not stream data of command" << command;
        return false;
    }

    bool success = false;
    QByteArray rawDataSend = streamCommandData(command->getHeader(), *commandRawData);
    if (Q_LIKELY(! rawDataSend.isEmpty()))
    {
        if (_logRawData)
        {
            qDebug() << "Sending" << BitField::toHex(rawDataSend, ':', 16) << command;
        }

        qint64 bytesWritten = _device->write(rawDataSend);
        success = bytesWritten == rawDataSend.size();
        if (! success)
        {
            qWarning() << "Wrote" << bytesWritten << "instead of" << rawDataSend.size();
        }
    }

    return success;
}

Command* AbstractDeviceCommandsQueue::makeRequestCommand(const CommandHeader* header)
{
    Q_UNUSED(header);
    return nullptr;
}

void AbstractDeviceCommandsQueue::fixBufferStandard(
    QByteArray& buffer,
    const QByteArray& markerStart,
    const QByteArray& markerEnd) const
{
    int indexOfEnd = -1;
    if (! markerEnd.isEmpty())
    {
        indexOfEnd = buffer.indexOf(markerEnd);
    }

    // Do not fix buffer by giving a similar buffer...
    int indexOfStart = -1;
    if (! markerStart.isEmpty())
    {
        indexOfStart = buffer.mid(1).indexOf(markerStart);
        if (indexOfStart >= 0)
        {
            indexOfStart++;
        }
    }

    if (indexOfEnd >= 0 && indexOfStart >= 0)
    {
        // If we both have a start and an end, keep the first occuring one
        if (indexOfEnd < indexOfStart)
        {
            indexOfStart = -1;
        }
        else
        {
            indexOfEnd = -1;
        }
    }

    if (indexOfEnd >= 0)
    {
        buffer = buffer.mid(indexOfEnd + 1);
    }
    else if (indexOfStart >= 0)
    {
        buffer = buffer.mid(indexOfStart);
    }
    else
    {
        // There is nothing we can hold on to, give up
        buffer.clear();
    }
}

Command* AbstractDeviceCommandsQueue::matchAnsweredCommand(const CommandHeader* header)
{
    QVector<Command*> candidate_commands;
    if (handleParallelCommands())
    {
        candidate_commands = getCommands().toVector();
    }
    else if (! getCommands().empty())
    {
        candidate_commands.push_back(getCommands().head());
    }

    for (Command* candidate_command : candidate_commands)
    {
        if (candidate_command->getHeader()->matches(header))
        {
            // Ok, this is the answer we were waiting for
            return candidate_command;
        }
    }

    return nullptr;
}

void AbstractDeviceCommandsQueue::onReadyRead()
{
    _buffer.append(_device->readAll());
    parseBuffer();
}

void AbstractDeviceCommandsQueue::parseBuffer()
{
    if (! _buffer.isEmpty())
    {
        if (_logRawData)
        {
            qDebug() << "Parsing" << BitField::toHex(_buffer, ':', 16);
        }

        CommandHeader* header = nullptr;
        QByteArray commandRawData;
        quint16 consumedBytes;
        DataParseResult result = unstreamReceivedData(_buffer, consumedBytes, header, commandRawData);

        switch (result)
        {
            case DataParseResult::Success:
                _buffer = _buffer.mid(consumedBytes);
                treatCommandData(header, commandRawData);
                parseBuffer(); // Do it again in case many commands have stacked in the buffer
                break;
            case DataParseResult::NotEnoughData:
                // Not much to do here, just wait for more data
                break;
            case DataParseResult::WrongData:
                // Data stream is corrupted, clear everything
                qWarning() << "Corrupted data, fixing buffer";
                fixBuffer(_buffer);
                parseBuffer(); // Try again in case there is some remaining data
                break;
        }
    }
}

void AbstractDeviceCommandsQueue::treatCommandData(const CommandHeader* header, const QByteArray& commandRawData)
{
    Command* command = matchAnsweredCommand(header);
    CommandDataType::Enum dataType;
    if (command)
    {
        // This is an existing command being answered
        dataType = CommandDataType::Answer;
    }
    else
    {
        // This is a new request
        dataType = CommandDataType::Request;
        command = makeRequestCommand(header);
        if (! command)
        {
            qWarning() << "Unable to create request command for header" << header;
            return;
        }
    }

    if (command->unstreamCommandData(commandRawData, dataType))
    {
        if (dataType == CommandDataType::Request)
        {
            onCommandReceived(command);
        }
    }
    else
    {
        qWarning() << "Error when parsing data for command" << command;
        command->manualFail();
    }
}
