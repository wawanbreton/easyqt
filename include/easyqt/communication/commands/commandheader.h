#pragma once

#include <QByteArray>

class CommandHeader
{
public:
    using Ptr = std::shared_ptr<CommandHeader>;
    using ConstPtr = std::shared_ptr<const CommandHeader>;

public:
    explicit CommandHeader() = default;

    virtual bool matches(const std::shared_ptr<const CommandHeader>& header) const = 0;

    virtual std::optional<QByteArray> streamData() const = 0;

    virtual QString toString() const = 0;

    friend QDebug operator<<(QDebug dbg, const std::shared_ptr<const CommandHeader>& header);
};
