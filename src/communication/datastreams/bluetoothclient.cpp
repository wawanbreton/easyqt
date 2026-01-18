#include "bluetoothclient.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include <QFile>
#include <QTimer>

#include "platform/utils/data/configuration.h"
#include "platform/utils/io/bluetoothreadinesswaiter.h"


BluetoothClient::BluetoothClient(const QBluetoothUuid &serviceUuid, QObject *parent) :
    AbstractDataStreamProvider(parent),
    _serviceUuid(serviceUuid),
    _socketConnectionTimer(new QTimer(this))
{
    _socketConnectionTimer->setInterval(5000);
    _socketConnectionTimer->setSingleShot(true);

    QBluetoothAddress serverAddress;
    QString serverName;
    getServer(serverAddress, serverName);

    if(!serverAddress.isNull())
    {
        moveToState(WaitingForDevice);
    }
    else
    {
        moveToState(NoConfig);
    }
}

void BluetoothClient::setServer(const QBluetoothAddress &address, const QString &name)
{
    Configuration config("network");
    config.setValue("bluetooth/server-address", ParamType::String, address.toString());
    config.setValue("bluetooth/server-name", ParamType::String, name);
    config.write();

    moveToState(Discovery);
}

void BluetoothClient::getServer(QBluetoothAddress &address, QString &name)
{
    Configuration config("network");
    address = QBluetoothAddress(
                config.value("bluetooth/server-address", ParamType::String).toString());
    name = config.value("bluetooth/server-name", ParamType::String).toString();
}

void BluetoothClient::prepareSocket()
{
    _socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(_socket, &QBluetoothSocket::connected,
            this,    [this]() { moveToState(Connected); } );
    connect(_socket, &QBluetoothSocket::disconnected,
            this,    [this]() { moveToState(WaitReconnect); } );
    connect(_socket, static_cast<void (QBluetoothSocket::*)(QBluetoothSocket::SocketError)>(&QBluetoothSocket::error),
            this,    [this]() { moveToState(WaitReconnect); } );
    connect(_socketConnectionTimer, &QTimer::timeout,
            _socket,                &QBluetoothSocket::abort);
}

void BluetoothClient::moveToState(BluetoothClient::ConnectionState state)
{
    if(state != Connected)
    {
        // In any other case, an existing socket should be discarded...
        if(_socket)
        {
            removeDevice(_socket);
            disconnect(_socket, nullptr, this, nullptr);
            _socket->abort();
            _socket->deleteLater();
            _socket = nullptr;
        }
    }

    _state = state;

    switch(_state)
    {
        case Idle:
            qCritical() << "We whould never go back to Idle state";
            break;
        case WaitingForDevice:
            connect(new BluetoothReadinessWaiter(this), &BluetoothReadinessWaiter::bluetoothReady,
                    this,                               [this]() { moveToState(Connecting); });
            break;
        case NoConfig:
            // Nothing to do, just wait for an external call to setServer
            break;
        case WaitReconnect:
            QTimer::singleShot(1000, this, [this]() { moveToState(Connecting); });
            break;
        case Connecting:
        {
            QBluetoothAddress serverAddress;
            QString serverName;
            getServer(serverAddress, serverName);

            if(serverAddress.isNull())
            {
                moveToState(NoConfig);
            }
            else
            {
                prepareSocket();
                _socketConnectionTimer->start();
                _socket->connectToService(serverAddress, _serviceUuid);
            }
            break;
        }
        case Connected:
        {
            if(_socket)
            {
                _socketConnectionTimer->stop();
                addStream(_socket, _socket->peerName());
            }
            else
            {
                qCritical() << "Moving to Connected state but there is no registered socket ?!";
            }
            break;
        }
        case Discovery:
        {
            _serverFound = false;

            QBluetoothDeviceDiscoveryAgent *agent = new QBluetoothDeviceDiscoveryAgent(this);
            connect(agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                    this,  &BluetoothClient::onDeviceDiscovered);
            connect(agent, &QBluetoothDeviceDiscoveryAgent::finished,
                    this,  &BluetoothClient::onDiscoveryFinished);
            connect(agent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
                    this,  &BluetoothClient::onDiscoveryFinished);
            connect(agent, &QBluetoothDeviceDiscoveryAgent::canceled,
                    this,  &BluetoothClient::onDiscoveryFinished);
            connect(agent, &QBluetoothDeviceDiscoveryAgent::finished,
                    agent, &QBluetoothDeviceDiscoveryAgent::deleteLater);
            connect(agent, &QBluetoothDeviceDiscoveryAgent::canceled,
                    agent, &QBluetoothDeviceDiscoveryAgent::deleteLater);

            agent->start(QBluetoothDeviceDiscoveryAgent::ClassicMethod);
            break;
        }
    }
}

void BluetoothClient::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    QBluetoothAddress serverAddress;
    QString serverName;
    getServer(serverAddress, serverName);

    qInfo() << "device discovered" << info.name() << info.address().toString() << serverAddress;

    if(info.address() == serverAddress)
    {
        _serverFound = true;

        QBluetoothDeviceDiscoveryAgent *agent =
                qobject_cast<QBluetoothDeviceDiscoveryAgent *>(sender());
        if(agent)
        {
            agent->stop();
        }
        else
        {
            qCritical() << "Sender is not a QBluetoothDeviceDiscoveryAgent";
        }
    }
}

void BluetoothClient::onDiscoveryFinished()
{
    moveToState(Connecting);
    emit discoveryFinished(_serverFound);
}
