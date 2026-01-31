#include "communication/commands/commandheader.h"

#include <QDebug>


QDebug operator<<(QDebug dbg, const std::shared_ptr<const CommandHeader>& header)
{
    QDebugStateSaver saver(dbg);

    dbg.noquote() << "Header(" << header->toString() << ")";

    return dbg;
}
