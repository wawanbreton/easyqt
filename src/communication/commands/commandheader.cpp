#include "communication/commands/commandheader.h"

#include <QDebug>


QDebug operator<<(QDebug dbg, const CommandHeader* command)
{
    QDebugStateSaver saver(dbg);

    dbg.noquote() << "Header(" << command->toString() << ")";

    return dbg;
}
