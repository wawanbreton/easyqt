#pragma once

#include <QObject>
#include <QTimer>
#include <QVariant>

#include "easyqt/communication/core/commanddatatype.h"

class CommandHeader;

/*! @brief Class for all communication commands, whether simulated or physical. It is actually just
 *         a container for request/answer data and a context-object for signal connection. */
class Command : public QObject
{
    Q_OBJECT

public:
    /*! @brief Constructor
     *  @param header The command header
     *  @param timeout The command answering timeout, or <=0 if there is no timeout
     *  @param parent The parent container */
    explicit Command(std::shared_ptr<const CommandHeader> header, QObject* parent = nullptr);

    void setTimeout(const int timeout);

    std::shared_ptr<const CommandHeader> getHeader() const
    {
        return header_;
    }

    virtual std::optional<QByteArray> streamData(CommandDataType::Enum dataType) const;

    bool unstreamCommandData(const QByteArray& rawData, CommandDataType::Enum dataType);

    void onSent();

    void manualFail()
    {
        emit error();
    }

    const bool expectsAnswer() const
    {
        return expects_answer_;
    }

    void setExpectsAnswer(bool expects_answer)
    {
        expects_answer_ = expects_answer;
    }

    friend QDebug operator<<(QDebug dbg, const Command* command);

protected:
    virtual bool unstreamCommandDataImpl(const QByteArray& rawData, CommandDataType::Enum dataType);

signals:
    void sent();

    void answerReceived();

    void error();

private:
    std::shared_ptr<const CommandHeader> header_;
    QTimer* timer_{ nullptr };
    bool expects_answer_{ false };
};
