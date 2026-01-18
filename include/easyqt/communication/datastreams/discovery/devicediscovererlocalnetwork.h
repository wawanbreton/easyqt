#pragma once

#include "platform/communication/datastreams/discovery/abstractdevicediscoverer.h"

class MultiCastUdpSocket;

class DeviceDiscovererLocalNetwork : public AbstractDeviceDiscoverer
{
    Q_OBJECT

    public:
        explicit DeviceDiscovererLocalNetwork(QObject *parent = nullptr);

        virtual void start() override;

    private:
        void onDataGramReceived(const QByteArray &data, const QHostAddress &address);

        void sendBroadcastMessage();

    private:
        MultiCastUdpSocket *_multiCastSocket{nullptr};
        QByteArray _broadcastMessage;
};
