#pragma once

#include <QObject>

#warning verifier les constructions/destructions, et/ou utiliser un shared_ptr
class CommandHeader : public QObject
{
public:
    explicit CommandHeader(QObject* parent = nullptr)
        : QObject(parent)
    {
    }

    // Required to mark the class as polymorphic
    virtual ~CommandHeader() = default;

    virtual bool matches(const CommandHeader* other) const = 0;

    virtual std::optional<QByteArray> streamData() const = 0;

    virtual QString toString() const = 0;

    friend QDebug operator<<(QDebug dbg, const CommandHeader* header);
};
