#pragma once

#include "easyqt/communication/commands/commandheader.h"


class IdBasedHeader : public CommandHeader
{
    Q_OBJECT

public:
    explicit IdBasedHeader(const quint32 id, QObject* parent = nullptr);

    quint32 getId() const
    {
        return id_;
    }

    virtual bool matches(const CommandHeader* other) const override;

private:
    const quint32 id_;
};
