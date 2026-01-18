#pragma once

#include <QObject>

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QBluetoothLocalDevice>

#include "platform/communication/datastreams/bluetoothclientstate.h"

class QBluetoothSocket;
class QBluetoothUuid;
class QBluetoothLocalDevice;
class QTimer;

class BluetoothPairedClient : public QObject
{
    Q_OBJECT

    public:
        explicit BluetoothPairedClient(const QBluetoothAddress &serverAddress,
                                       const QBluetoothUuid &serviceUuid,
                                       QObject *parent = nullptr);

        void start();

        QBluetoothSocket *getSocket() const { return _socket; }

        const QBluetoothAddress &getAddress() const { return _serverAddress; }

        bool isOver() const;

        bool isConnected() const { return _state == BluetoothClientState::Connected; }

    signals:
        void stateChanged(const BluetoothClientState::Enum &state);

    private:
        void prepareSocket();

        void moveToState(const BluetoothClientState::Enum &state);

        void onDeviceReady();

        void onPairingFinished(const QBluetoothAddress &address,
                               const QBluetoothLocalDevice::Pairing &pairing);

    private:
        const QBluetoothAddress _serverAddress;
        const QBluetoothUuid _serviceUuid;
        QBluetoothSocket *_socket{nullptr};
        QTimer *_socketConnectionTimer{nullptr};
        BluetoothClientState::Enum _state{BluetoothClientState::Idle};
        QBluetoothLocalDevice *_device{nullptr};
        bool _serverFound{false};
};
