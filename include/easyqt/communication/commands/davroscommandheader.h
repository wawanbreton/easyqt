#pragma once

#include "platform/communication/commands/commandheader.h"

class DavrosCommandHeader : public CommandHeader
{
    public:
        DavrosCommandHeader(const quint32 uniqueId, const quint32 &commandId) :
            CommandHeader(commandId),
            _uniqueId(uniqueId)
        {}

        const quint32 &getUniqueId() const { return _uniqueId; }

    private:
        const quint32 _uniqueId;
};
