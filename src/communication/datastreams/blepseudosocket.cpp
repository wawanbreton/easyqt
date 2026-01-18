#include "blepseudosocket.h"

#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QTimer>

#include "platform/constants.h"
#include "platform/utils/data/bitfield.h"
#include "platform/utils/qt/assertutils.h"


BLEPseudoSocket::BLEPseudoSocket(QObject *parent, const QBluetoothAddress &address) :
    AbstractBufferizedDevice(parent)
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    auto devices = QBluetoothLocalDevice::allDevices();
    if(!devices.isEmpty())
    {
        _controller = QLowEnergyController::createCentral(address, devices.first().address(), this);
        qInfo() << "Creating BLE central with new method";
    #else
    // Compatibility with older Qt version, but less likely to work in all situations !
    QBluetoothDeviceInfo deviceInfo(address,
                                    ConstantsCommunication::bluetoothDeviceName,
                                    0);

    _controller = QLowEnergyController::createCentral(deviceInfo, this);
    qInfo() << "Creating BLE central with old method";
    #endif

        connect(_controller, &QLowEnergyController::connected,
                this,        &BLEPseudoSocket::onDeviceConnected);
        connect(_controller, &QLowEnergyController::disconnected,
                this,        &BLEPseudoSocket::onDisconnected);
        connect(_controller, qOverload<QLowEnergyController::Error>(&QLowEnergyController::error),
                this,        &BLEPseudoSocket::onConnectionError);
        connect(_controller, &QLowEnergyController::serviceDiscovered,
                this,        &BLEPseudoSocket::onServiceDiscovered);
        connect(_controller, &QLowEnergyController::discoveryFinished,
                this,        &BLEPseudoSocket::onServicesDiscoveryFinished);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    }
    else
    {
        qWarning() << "No local controller found";
    }
    #endif
}

BLEPseudoSocket::~BLEPseudoSocket()
{
    close();
}

void BLEPseudoSocket::connectToDevice()
{
    if(_controller && !openMode().testFlag(QIODevice::ReadWrite) && _status == Idle)
    {
        qInfo() << "Trying to connect to " << remoteAddress();
        _status = Connecting;
        _controller->connectToDevice();
    }
}

QBluetoothAddress BLEPseudoSocket::remoteAddress() const
{
    return _controller ? _controller->remoteAddress() : QBluetoothAddress();
}

void BLEPseudoSocket::close()
{
    if(_controller)
    {
        if(_controller->state() != QLowEnergyController::UnconnectedState)
        {
            qInfo() << "Disconnecting from" << _controller->remoteAddress();
            _controller->disconnectFromDevice();
        }
        _controller->deleteLater();
        _controller = nullptr;
    }

    if(_service)
    {
        _service->deleteLater();
        _service = nullptr;
    }

    AbstractBufferizedDevice::close();
}

qint64 BLEPseudoSocket::writeData(const char *data, qint64 maxSize)
{
    if(_service)
    {
        QByteArray dataToSend(data, static_cast<int>(maxSize));
        _service->writeCharacteristic(_characteristic,
                                      dataToSend,
                                      QLowEnergyService::WriteWithResponse);
        return maxSize;
    }

    return 0;
}

void BLEPseudoSocket::onDeviceConnected()
{
    qInfo() << "Connected to device" << remoteAddress();
    _status = DetectingServices;
    _controller->discoverServices();
}

void BLEPseudoSocket::onServiceDiscovered(const QBluetoothUuid &service)
{
    if(!_service && service == ConstantsCommunication::bleServiceUuid)
    {
        _service = _controller->createServiceObject(service, this);

        connect(_service, &QLowEnergyService::stateChanged,
                this,     &BLEPseudoSocket::onServiceStateChanged);

        _service->discoverDetails();
    }
}

void BLEPseudoSocket::onServicesDiscoveryFinished()
{
    if(!_service)
    {
        qWarning() << "Expected service was not found";
        _status = Error;
        emit error(QLowEnergyController::ConnectionError);
    }
}

void BLEPseudoSocket::onServiceStateChanged(const QLowEnergyService::ServiceState &state)
{
    if(_service && state == QLowEnergyService::ServiceDiscovered)
    {
        for(auto characteristic : _service->characteristics())
        {
            if(characteristic.uuid() == ConstantsCommunication::bleCharacteristicUuid)
            {
                _characteristic = characteristic;

                // Enable notifications for characteristic change
                auto descriptorConfig =
                        _characteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
                if(descriptorConfig.isValid())
                {
                    _service->writeDescriptor(descriptorConfig, QByteArray::fromHex("0100"));

                    connect(_service, &QLowEnergyService::characteristicChanged,
                            this,     &BLEPseudoSocket::onCharacteristicChanged);

                    setOpenMode(QIODevice::ReadWrite);
                }
                else
                {
                    qWarning() << "Unable to activate characteristic notification";

                    setOpenMode(QIODevice::WriteOnly);
                }

                _status = Connected;
                #ifdef Q_OS_WIN
                // It seems that we have to wait a bit before communication is properly set up
                QTimer::singleShot(3000, this, &BLEPseudoSocket::connected);
                #else
                emit connected();
                #endif

                return;
            }
        }

        qWarning() << "Expected characteristic was not found";
    }
}

void BLEPseudoSocket::onDisconnected()
{
    if(_controller)
    {
        _controller->deleteLater();
        _controller = nullptr;
    }

    if(_service)
    {
        _service->deleteLater();
        _service = nullptr;
    }

    setOpenMode(QIODevice::NotOpen);

    emit disconnected();
}

void BLEPseudoSocket::onCharacteristicChanged(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &newValue)
{
    if(characteristic.uuid() == ConstantsCommunication::bleCharacteristicUuid)
    {
        addToReadBuffer(newValue);
    }
}

void BLEPseudoSocket::onConnectionError(const QLowEnergyController::Error &connectionError)
{
    if(_status == Connecting && connectionError == QLowEnergyController::UnknownRemoteDeviceError)
    {
        // In this case, it may be required to process a discovery to re-discover the device
        qInfo() << "Trying to discover " << remoteAddress();
        _status = Discovering;

        auto discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
        connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                this,           &BLEPseudoSocket::onDeviceDiscovered);
        connect(discoveryAgent,
                qOverload<QBluetoothDeviceDiscoveryAgent::Error>(&QBluetoothDeviceDiscoveryAgent::error),
                this,
                &BLEPseudoSocket::onDiscoverError);
        connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
                this,           &BLEPseudoSocket::onDeviceDiscoveryFinished);

        discoveryAgent->setLowEnergyDiscoveryTimeout(10000);
        discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    }
    else
    {
        _status = Error;
        emit error(connectionError);
    }
}

void BLEPseudoSocket::onDiscoverError(QBluetoothDeviceDiscoveryAgent::Error discoveryError)
{
    if(_status == Discovering)
    {
        qWarning() << "Discovery error" << discoveryError;
        emit error(QLowEnergyController::UnknownRemoteDeviceError);
    }
}

void BLEPseudoSocket::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    if(info.address() == remoteAddress() && _status == Discovering)
    {
        qInfo() << "Trying to reconnect to " << remoteAddress();
        _status = Reconnecting;
        _controller->connectToDevice();
    }
}

void BLEPseudoSocket::onDeviceDiscoveryFinished()
{
    if(_status == Discovering)
    {
        qWarning() << "Device not found" << remoteAddress();
        emit error(QLowEnergyController::UnknownRemoteDeviceError);
    }
}
