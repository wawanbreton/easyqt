#pragma once

#include "platform/communication/datastreams/discovery/abstractdevicediscoverer.h"

#include <QBluetoothDeviceDiscoveryAgent>

class DeviceDiscovererBLE : public AbstractDeviceDiscoverer
{
    Q_OBJECT

    public:
        explicit DeviceDiscovererBLE(QObject *parent);

        virtual void start() override;

    private:
        void onDiscoverError(QBluetoothDeviceDiscoveryAgent::Error error);

        void onDeviceDiscovered(const QBluetoothDeviceInfo &info);
};

