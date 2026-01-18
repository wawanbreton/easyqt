#pragma once

#include "platform/utils/data/abstractbufferizeddevice.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyController>
#include <QLowEnergyService>

class BLEPseudoSocket : public AbstractBufferizedDevice
{
    Q_OBJECT

    public:
        explicit BLEPseudoSocket(QObject *parent, const QBluetoothAddress &address);

        virtual ~BLEPseudoSocket();

        void connectToDevice();

        virtual bool isSequential() const override { return true; }

        QBluetoothAddress remoteAddress() const;

        virtual void close() override;

    signals:
        void connected();

        void disconnected();

        void error(const QLowEnergyController::Error &error);

    protected:
        virtual qint64 writeData(const char *data, qint64 maxSize) override;

    private:
        enum Status
        {
            Idle,
            Connecting,
            Discovering,
            Reconnecting,
            NotDiscovered,
            Error,
            DetectingServices,
            Connected,
        };

    private:
        void onDeviceConnected();

        void onServiceDiscovered(const QBluetoothUuid &service);

        void onServicesDiscoveryFinished();

        void onServiceStateChanged(const QLowEnergyService::ServiceState &state);

        void onDisconnected();

        void onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic,
                                     const QByteArray &newValue);

        void onConnectionError(const QLowEnergyController::Error &connectionError);

        void onDiscoverError(QBluetoothDeviceDiscoveryAgent::Error error);

        void onDeviceDiscovered(const QBluetoothDeviceInfo &info);

        void onDeviceDiscoveryFinished();

    private:
        Status _status{Idle};
        QLowEnergyController *_controller{nullptr};
        QLowEnergyService *_service{nullptr};
        QLowEnergyCharacteristic _characteristic;
};

