#pragma once

#include <QObject>
#include <QVariant>
#include <functional>

using SlotAnswerType = std::function<void(const QList<QVariant>&)>;
using SlotNoArgType = std::function<void()>;

class QIODevice;

class Command;
class CommandHeader;
class AbstractCommandsQueue;

/*! @brief Basic class for all communication interfaces. The basic job of an interface is to
 *         wrap and unwrap messages to advertise nice methods to communicate with a specific object.
 *
 *  The child classes should :
 *    * Add methods for every message, and call the sendCommand() with proper arguments
 *    * Override onCommandReceived method and treat all kinds of received messages to send
 *      proper event
 *    * Give the proper commands queue to constructor to select a communication method
 */
class AbstractCommunicationInterface : public QObject
{
    Q_OBJECT

public:
    const AbstractCommandsQueue* getQueue() const
    {
        return _queue;
    }

    AbstractCommandsQueue* accessQueue()
    {
        return _queue;
    }

    QIODevice* accessDevice();

    const QIODevice* getDevice() const;

    virtual void cancelCurrentCommand();

    Command* makeRequest(const std::shared_ptr<const CommandHeader>& header, const int timeout = 0);

    Command* makeEvent(const std::shared_ptr<const CommandHeader>& header);

protected:
    /*! @brief Constructor
     *  @param queue The object holding the commands queue
     *  @param parent The parent container */
    explicit AbstractCommunicationInterface(AbstractCommandsQueue* queue, QObject* parent = nullptr);

    /*! @brief Pushes a new command to the queue, and sent it as soon as possible
     *  @param id The ID of the command to be sent
     *  @param dataRequest The command payload data
     *  @param receiver The object receiving the signals
     *  @param slotAnswer Slot to be called when the command has been sent.
     *                    It should take up to two lists of QList<QVariant> argument :
     *                    the received answer data, and the originally sent data.
     *  @param slotError Slot to be called when an error occurs while sending the command.
     *                   It should take no argument.
     *  @param slotSent Slot to be called when the command has been sent.
     *                  It should take no argument. */
    void sendRequest(
        Command* command,
        QObject* receiver = nullptr,
        const SlotNoArgType& slotError = nullptr,
        const SlotNoArgType& slotSent = nullptr);

    void sendRequestEmptyAnswer(
        Command* command,
        QObject* receiver = nullptr,
        const SlotNoArgType& slotAnswer = nullptr,
        const SlotNoArgType& slotError = nullptr,
        const SlotNoArgType& slotSent = nullptr);

    /*! @brief Pushes a new event to the queue, and sent it as soon as possible. An event is a
     *         command for which an answer is not expected.
     *  @param id The ID of the command to be sent
     *  @param dataRequest The command payload data
     *  @param receiver The object receiving the signals
     *  @param slotError Slot to be called when an error occurs while sending the command.
     *                   It should take no argument.
     *  @param slotSent Slot to be called when the command has been sent.
     *                  It should take no argument. */
    void sendEvent(
        Command* command,
        QObject* receiver = nullptr,
        const SlotNoArgType& slotError = nullptr,
        const SlotNoArgType& slotSent = nullptr);

    /*! @brief Method called when a spontaneous command is received
     *  @param command The received command
     *  @return True if the command has been recognized and expects an answer to be set later
     *          False if the command has not been recognized, or if it is a request-only command
     */
    virtual bool onCommandReceivedImpl(Command* command);

private:
    void onCommandReceived(Command* command);

    /*! @brief Pushes a new command to the queue, and sent it as soon as possible
     *  @param id The ID of the command to be sent
     *  @param dataRequest The command payload data
     *  @param receiver The object receiving the signals
     *  @param slotAnswer Slot to be called when the command has been sent.
     *                    It should take up to two lists of QList<QVariant> argument :
     *                    the received answer data, and the originally sent data.
     *  @param slotError Slot to be called when an error occurs while sending the command.
     *                   It should take no argument.
     *  @param slotSent Slot to be called when the command has been sent.
     *                  It should take no argument.
     *  @param timeout The command answering timeout : <0 for no timeout,
     *                                                 =0 for default timeout
     *                                                 >0 for a specific timeout
     *  @param expectsAnswer Indicates whether the commands expects and answer (it is a
     *                       request/answer) or not (it is an event) */
    void sendCommand(
        Command* command,
        QObject* receiver,
        const SlotNoArgType& slotError,
        const SlotNoArgType& slotSent,
        const bool expectsAnswer);

private:
    AbstractCommandsQueue* const _queue{ nullptr };
};
