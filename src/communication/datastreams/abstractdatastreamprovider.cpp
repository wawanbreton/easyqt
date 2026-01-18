#include "abstractdatastreamprovider.h"

#include <QDebug>
#include "platform/utils/qt/debug.h"


AbstractDataStreamProvider::AbstractDataStreamProvider(QObject *parent) :
    DeviceAggregator(parent)
{
}

void AbstractDataStreamProvider::addStream(QIODevice *stream, const QString &humanReadableName)
{
    DeviceAggregator::addDevice(stream,
                                humanReadableName,
                                _mode == ModeMergedStreams);
}
