#include "bluetoothpairedclient.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include <QFile>
#include <QTimer>

#include "platform/utils/data/configuration.h"
#include "platform/utils/io/bluetoothreadinesswaiter.h"


BluetoothPairedClient::BluetoothPairedClient(const QBluetoothAddress &serverAddress,
                                             const QBluetoothUuid &serviceUuid,
                                             QObject *parent) :
    QObject(parent),
    _serverAddress(serverAddress),
    _serviceUuid(serviceUuid),
    _socketConnectionTimer(new QTimer(this))
{
    _socketConnectionTimer->setInterval(5000);
    _socketConnectionTimer->setSingleShot(true);
}

void BluetoothPairedClient::start()
{
    if(_state == BluetoothClientState::Idle)
    {
        moveToState(BluetoothClientState::WaitingForDevice);
    }
}

bool BluetoothPairedClient::isOver() const
{
    return _state == BluetoothClientState::Connected ||
           _state == BluetoothClientState::ConnectionFailed ||
           _state == BluetoothClientState::NotAvailable ||
           _state == BluetoothClientState::PairingFailed;
}

void BluetoothPairedClient::prepareSocket()
{
    _socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(_socket, &QBluetoothSocket::connected,
            this,    [this]() { moveToState(BluetoothClientState::Connected); } );
    connect(_socket, &QBluetoothSocket::disconnected,
            this,    [this]() { moveToState(BluetoothClientState::ConnectionFailed); } );
    connect(_socket, static_cast<void (QBluetoothSocket::*)(QBluetoothSocket::SocketError)>(&QBluetoothSocket::error),
            this,    [this]() { moveToState(BluetoothClientState::ConnectionFailed); } );
    connect(_socketConnectionTimer, &QTimer::timeout,
            _socket,                &QBluetoothSocket::abort);
}

void BluetoothPairedClient::moveToState(const BluetoothClientState::Enum &state)
{
    if(state != BluetoothClientState::Connected)
    {
        // In any other case, an existing socket should be discarded...
        if(_socket)
        {
            disconnect(_socket, nullptr, this, nullptr);
            _socket->abort();
            _socket->deleteLater();
            _socket = nullptr;
        }
    }
    _socketConnectionTimer->stop();

    _state = state;

    emit stateChanged(_state);

    switch(_state)
    {
        case BluetoothClientState::Idle:
            qCritical() << "We should never go back to Idle state";
            break;

        case BluetoothClientState::WaitingForDevice:
            connect(new BluetoothReadinessWaiter(this), &BluetoothReadinessWaiter::bluetoothReady,
                    this,                               &BluetoothPairedClient::onDeviceReady);
            break;

        case BluetoothClientState::Pairing:
        {
            if(_device)
            {
                _device->requestPairing(_serverAddress, QBluetoothLocalDevice::Paired);
            }
            else
            {
                moveToState(BluetoothClientState::NotAvailable);
            }
            break;
        }

        case BluetoothClientState::Connecting:
        {
            prepareSocket();
            _socketConnectionTimer->start();
            _socket->connectToService(_serverAddress, _serviceUuid);
            break;
        }

        case BluetoothClientState::Connected:
        {
            if(!_socket)
            {
                qCritical() << "Moving to Connected state but there is no registered socket ?!";
            }
            break;
        }

        case BluetoothClientState::PairingFailed:
        case BluetoothClientState::ConnectionFailed:
        case BluetoothClientState::NotAvailable:
            break;
    }
}

void BluetoothPairedClient::onDeviceReady()
{
    if(_device)
    {
        _device->deleteLater();
        _device = nullptr;
    }

    QList<QBluetoothHostInfo> devices = QBluetoothLocalDevice::allDevices();
    if(!devices.isEmpty())
    {
        QBluetoothHostInfo hostInfo = devices.first();
        _device = new QBluetoothLocalDevice(hostInfo.address());
        connect(_device, &QBluetoothLocalDevice::pairingFinished,
                this,    &BluetoothPairedClient::onPairingFinished);
    }

    if(!_device)
    {
        moveToState(BluetoothClientState::NotAvailable);
    }
    else if(_device->pairingStatus(_serverAddress) == QBluetoothLocalDevice::Unpaired)
    {
        moveToState(BluetoothClientState::Pairing);
    }
    else
    {
        moveToState(BluetoothClientState::Connecting);
    }
}

void BluetoothPairedClient::onPairingFinished(const QBluetoothAddress &address,
                                              const QBluetoothLocalDevice::Pairing &pairing)
{
    if(address == _serverAddress)
    {
        if(pairing == QBluetoothLocalDevice::Unpaired)
        {
            moveToState(BluetoothClientState::PairingFailed);
        }
        else
        {
            moveToState(BluetoothClientState::Connecting);
        }
    }
}
