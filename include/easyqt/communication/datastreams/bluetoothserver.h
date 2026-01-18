#pragma once

#include "platform/communication/datastreams/abstractdatastreamprovider.h"

#include <QUuid>

class BluetoothServer : public AbstractDataStreamProvider
{
    Q_OBJECT

    public:
        explicit BluetoothServer(const QString &serviceName,
                                 const QString &serviceDescription,
                                 const QUuid &serviceUuid,
                                 const int &discoverableTimeout = -1,
                                 QObject *parent = nullptr);

    private:
        void onNewBluetoothConnection();

        void onSocketDisconnected();

        void onBluetoothDeviceReady();

        void onDiscoverableTimeout();

    private:
        const QString _serviceName;
        const QString _serviceDescription;
        const QUuid _serviceUuid;
        const int _discoverableTimeout;
};
