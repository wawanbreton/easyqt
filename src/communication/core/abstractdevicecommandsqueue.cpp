#include "communication/core/abstractdevicecommandsqueue.h"

#include <QDebug>

#include "bitfield.h"
#include "communication/commands/command.h"
#include "communication/core/abstractcommanddatastreamer.h"


AbstractDeviceCommandsQueue::AbstractDeviceCommandsQueue(
    AbstractCommandDataStreamer* streamer,
    QIODevice* device,
    bool parallelCommands,
    int defaultTimeout,
    QObject* parent,
    const bool& logRawData)
    : AbstractCommandsQueue(parallelCommands, defaultTimeout, parent)
    , _streamer(streamer)
    , _device(device)
    , _logRawData(logRawData)
{
    _streamer->setParent(this);

    connect(_device, &QIODevice::readyRead, this, &AbstractDeviceCommandsQueue::onReadyRead);
    connect(_device, &QIODevice::bytesWritten, this, &AbstractDeviceCommandsQueue::onDeviceBytesWritten);
}

bool AbstractDeviceCommandsQueue::sendCommandImpl(Command* command, CommandDataType::Enum dataType)
{
    bool success = false;

    bool dataPresent = false;
    QList<QVariant> userData = command->getData(dataType, &dataPresent);
    if (Q_LIKELY(dataPresent))
    {
        QByteArray commandRawData = _streamer->streamCommandData(command->getId(), userData, dataType);

        QByteArray rawDataSend = streamCommandData(command->getHeader(), commandRawData);
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
    }
    else
    {
        qCritical() << "Data" << dataType << "for command" << command << "has not ben set";
    }

    return success;
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

Command* AbstractDeviceCommandsQueue::matchAnsweredCommandBasic(const CommandHeader* header)
{
    if (! getCommands().isEmpty())
    {
        Command* currentCommand = getCommands().head();
        if (currentCommand->getId() == header->getId())
        {
            // Ok, this is the answer we were waiting for
            return currentCommand;
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
    QList<QVariant> dataSend;
    if (command)
    {
        // This is an existing command being answered
        dataType = CommandDataType::Answer;
        dataSend = command->getData(CommandDataType::Request);
    }
    else
    {
        // This is a new request
        dataType = CommandDataType::Request;
        command = new Command(header, 0, this);
    }

    command->setData(dataType, _streamer->unstreamCommandData(header->getId(), commandRawData, dataType, dataSend));

    if (dataType == CommandDataType::Request)
    {
        onCommandReceived(command);
    }
}
