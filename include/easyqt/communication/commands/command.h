#pragma once

#include <QObject>
#include <QTimer>
#include <QVariant>

#include "easyqt/communication/commands/commandheader.h"
#include "easyqt/communication/core/commanddatatype.h"

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
    explicit Command(const CommandHeader* header, int error, QObject* parent = nullptr);

    virtual ~Command();

    const CommandHeader* getHeader() const
    {
        return _header;
    }

    const quint32& getId() const
    {
        return getHeader()->getId();
    }

    QList<QVariant> getData(const CommandDataType::Enum& dataType, bool* dataPresent = nullptr) const;

    QList<QVariant> getRequestData() const
    {
        return getData(CommandDataType::Request);
    }

    bool hasData(CommandDataType::Enum dataType) const
    {
        return _data.contains(dataType);
    }

    void setData(const CommandDataType::Enum dataType, const QList<QVariant>& data);

    void setAnswer(const QList<QVariant>& data)
    {
        return setData(CommandDataType::Answer, data);
    }

    void onSent();

    void manualFail()
    {
        emit error();
    }

    const bool& expectsAnswer() const
    {
        return _expectsAnswer;
    }

    void setExpectsAnswer(bool expectsAnswer)
    {
        _expectsAnswer = expectsAnswer;
    }

    friend QDebug operator<<(QDebug dbg, const Command* command);

signals:
    void sent();

    void answerReceived(const QList<QVariant>& dataAnswer, const QList<QVariant>& dataRequest);

    void error();

private:
    const CommandHeader* _header;
    QMap<CommandDataType::Enum, QList<QVariant>> _data;
    QTimer* _timer{ nullptr };
    bool _expectsAnswer{ false };
};
