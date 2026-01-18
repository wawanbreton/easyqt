#include "bluetoothserver.h"

#include <QBluetoothHostInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothServer>
#include <QTimer>

#include "platform/utils/io/bluetoothreadinesswaiter.h"


BluetoothServer::BluetoothServer(const QString &serviceName,
                                 const QString &serviceDescription,
                                 const QUuid &serviceUuid,
                                 const int &discoverableTimeout,
                                 QObject *parent) :
    AbstractDataStreamProvider(parent),
    _serviceName(serviceName),
    _serviceDescription(serviceDescription),
    _serviceUuid(serviceUuid),
    _discoverableTimeout(discoverableTimeout)
{
    connect(new BluetoothReadinessWaiter(this), &BluetoothReadinessWaiter::bluetoothReady,
            this,                               &BluetoothServer::onBluetoothDeviceReady);
}

void BluetoothServer::onBluetoothDeviceReady()
{
    QList<QBluetoothHostInfo> devices = QBluetoothLocalDevice::allDevices();
    if(!devices.isEmpty())
    {
        QBluetoothHostInfo hostInfo = devices.first();

        // Make interface discoverable
        QBluetoothLocalDevice device(hostInfo.address());
        device.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
        if(_discoverableTimeout > 0)
        {
           QTimer::singleShot(_discoverableTimeout, this, &BluetoothServer::onDiscoverableTimeout);
        }

        // Start server
        QBluetoothServer *btServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol,
                                                          this);
        connect(btServer, &QBluetoothServer::newConnection,
                this,     &BluetoothServer::onNewBluetoothConnection);
        if(btServer->listen(hostInfo.address()))
        {
            QBluetoothServiceInfo serviceInfo;

            // Set service class
            QBluetoothServiceInfo::Sequence classId;
            classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
            serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,
                                     classId);
            classId.prepend(QVariant::fromValue(QBluetoothUuid(_serviceUuid)));

            serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

            // Set service description
            serviceInfo.setServiceName(_serviceName);
            serviceInfo.setServiceDescription(_serviceDescription);
            serviceInfo.setServiceProvider("tibot.fr");
            serviceInfo.setServiceUuid(QBluetoothUuid(_serviceUuid));

            // Set service discoverable
            QBluetoothServiceInfo::Sequence publicBrowse;
            publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
            serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList, publicBrowse);

            // Set connections information
            QBluetoothServiceInfo::Sequence protocolDescriptorList;
            QBluetoothServiceInfo::Sequence protocol;
            protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
            protocolDescriptorList.append(QVariant::fromValue(protocol));
            protocol.clear();
            protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
                     << QVariant::fromValue(quint8(btServer->serverPort()));
            protocolDescriptorList.append(QVariant::fromValue(protocol));
            serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                                     protocolDescriptorList);

            // Register set information
            serviceInfo.unregisterService();
            if(serviceInfo.registerService(devices.first().address()))
            {
                qInfo() << "Bluetooth server now listening on" << devices.first().address();
            }
            else
            {
                qWarning() << "Unable to register service : error" << btServer->error();
            }
        }
        else
        {
            qWarning() << "Unable to start listening on BT port : error" << btServer->error();
        }
    }
    else
    {
        qWarning() << "There is no bluetooth device";
    }
}

void BluetoothServer::onDiscoverableTimeout()
{
    QList<QBluetoothHostInfo> devices = QBluetoothLocalDevice::allDevices();
    if(!devices.isEmpty())
    {
        QBluetoothLocalDevice device(devices.first().address());
        device.setHostMode(QBluetoothLocalDevice::HostConnectable);
        qInfo() << "Turning BLuetooth discoverable off";
    }
}

void BluetoothServer::onNewBluetoothConnection()
{
    QBluetoothServer *btServer = qobject_cast<QBluetoothServer *>(sender());
    if(btServer)
    {
        QBluetoothSocket *socket = btServer->nextPendingConnection();
        if(socket)
        {
            connect(socket, &QBluetoothSocket::disconnected,
                    this,   &BluetoothServer::onSocketDisconnected);

            addStream(socket, socket->peerName());
        }
    }
    else
    {
        qCritical() << "Sender is not a QBluetoothServer" << sender();
    }
}

void BluetoothServer::onSocketDisconnected()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if(socket)
    {
        removeDevice(socket);
    }
    else
    {
        qCritical() << "Sender is not a QBluetoothSocket";
    }
}

