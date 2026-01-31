#pragma once

#include <QIODevice>

#include "easyqt/communication/core/abstractcommandsqueue.h"
#include "easyqt/communication/dataparseresult.h"


/*! @brief Abstract class that manages commands sent through a QIODevice. The commands can be
 *         serialized/deseralized, and the content will be sent through the device. It is the basis
 *         for any physical communication over USB, TCP, Bluetooth, ... */
class AbstractDeviceCommandsQueue : public AbstractCommandsQueue
{
    Q_OBJECT

public:
    explicit AbstractDeviceCommandsQueue(
        QIODevice* device,
        bool parallelCommands,
        int defaultTimeout,
        QObject* parent = nullptr,
        const bool& logRawData = false);

    const QIODevice* getDevice() const
    {
        return _device;
    }

    QIODevice* accessDevice() const
    {
        return _device;
    }

    virtual bool sendCommandImpl(Command* command, CommandDataType::Enum dataType) override;

protected:
    /*! @brief Create the appropriate request command when the given header has been received and no actual request
     *         could be matched to it
     *  @param header The received header
     *  @return The appropriate request command, or null if this header is not recognized */
    virtual Command* makeRequestCommand(const CommandHeader* header);

    /*! @brief Children classes should implement this method to generate the full data to be
     *         sent to the device (by adding headers and footers for example)
     *  @param header The header of the command to be send
     *  @param commandRawData The streamed command data */
    virtual QByteArray streamCommandData(const CommandHeader* header, const QByteArray& commandRawData) const = 0;

    /*! @brief Children classes should implement this method to parse raw received data, and
     *         send a proper signal at destination of the commands queue when relevant command
     *         data is received.
     *  @param buffer The actual data buffer to be parsed
     *  @param consumedBytes The bytes to be consumed from the buffer, in case the
     *                       result is Succes
     *  @param header The returned parsed command header
     *  @param commandRawData The returned raw command data
     *  @return The result indicating whether the parsing was complete, incomplete or in error
     */
    virtual DataParseResult unstreamReceivedData(
        const QByteArray& buffer,
        quint16& consumedBytes,
        CommandHeader*& header,
        QByteArray& commandRawData) const
        = 0;

    /*! @brief Children classes should implement this method to try and fix the buffer when
     *         something wrong is detected during communication. For example, looking for the
     *         message footer and remove everything before, or just clearing the buffer. */
    virtual void fixBuffer(QByteArray& buffer) const = 0;

    void fixBufferStandard(QByteArray& buffer, const QByteArray& markerStart, const QByteArray& markerEnd) const;

    /*! @brief Children classes should implement this method to try and find an actual command
     *         waiting for an answer, for which the given header could be the answser
     *  @param header The received command header
     *  @return The existing command matching the answer, or nullptr if there is none */
    Command* matchAnsweredCommand(const CommandHeader* header);

    virtual void onDeviceBytesWritten()
    {
    }

private:
    void onReadyRead();

    void parseBuffer();

    void treatCommandData(const CommandHeader* header, const QByteArray& commandRawData);

private:
    QIODevice* const _device{ nullptr };
    const bool _logRawData;
    QByteArray _buffer;
};
