#pragma once

#include "platform/communication/datastreams/abstractdatastreamprovider.h"

#include <QBluetoothServiceInfo>
#include <QBluetoothSocket>
#include <QBluetoothUuid>
#include <QElapsedTimer>
#include <QTimer>

class BluetoothClient : public AbstractDataStreamProvider
{
    Q_OBJECT

    public:
        explicit BluetoothClient(const QBluetoothUuid &serviceUuid, QObject *parent = nullptr);

        void setServer(const QBluetoothAddress &address, const QString &name);

        void getServer(QBluetoothAddress &address, QString &name);

        bool isTryingToConnect() const { return _state != NoConfig; }

    signals:
        void discoveryFinished(bool serverFound);

    private:
        typedef enum
        {
            Idle,
            WaitingForDevice,
            NoConfig,
            WaitReconnect,
            Connecting,
            Connected,
            Discovery
        } ConnectionState;

    private:
        void prepareSocket();

        void moveToState(ConnectionState state);

        void onDeviceDiscovered(const QBluetoothDeviceInfo &info);

        void onDiscoveryFinished();

    private:
        const QBluetoothUuid _serviceUuid;
        QBluetoothSocket *_socket{nullptr};
        QTimer *_socketConnectionTimer{nullptr};
        ConnectionState _state{Idle};
        bool _serverFound{false};
};
