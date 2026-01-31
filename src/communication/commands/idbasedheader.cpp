#include "communication/commands/idbasedheader.h"

#include "bitfield.h"


IdBasedHeader::IdBasedHeader(const quint32 id, QObject* parent)
    : CommandHeader(parent)
    , id_(id)
{
}

bool IdBasedHeader::matches(const CommandHeader* other) const
{
    auto other_id = qobject_cast<const IdBasedHeader*>(other);
    return other_id && other_id->id_ == id_;
}
