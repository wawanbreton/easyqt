#pragma once

#include "platform/utils/data/deviceaggregator.h"

class AbstractDataStreamProvider : public DeviceAggregator
{
    Q_OBJECT

    public:
        typedef enum
        {
            ModeMergedStreams,  // In this mode, the provider can be seen as if it was
                                // a single QIODevice (default)
            ModeManualStreams,  // In this mode, the provider takes care of
                                // registering/unregistering the devices, which are notified via
                                // newStream() and streamLost()
        } Mode;

    public:
        void setMode(Mode mode) { _mode = mode; }

    protected:
        explicit AbstractDataStreamProvider(QObject *parent = nullptr);

        void addStream(QIODevice *stream, const QString &humanReadableName);

    private:
        Mode _mode{ModeMergedStreams};
};
