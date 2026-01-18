#include "devicediscovererble.h"

#include <QBluetoothDeviceDiscoveryAgent>

#include "platform/communication/core/communicationmethod.h"
#include "platform/constants.h"


DeviceDiscovererBLE::DeviceDiscovererBLE(QObject *parent) :
    AbstractDeviceDiscoverer(parent)
{
}

void DeviceDiscovererBLE::start()
{
    auto discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this,           &DeviceDiscovererBLE::onDeviceDiscovered);
    connect(discoveryAgent,
            qOverload<QBluetoothDeviceDiscoveryAgent::Error>(&QBluetoothDeviceDiscoveryAgent::error),
            this,
            &DeviceDiscovererBLE::onDiscoverError);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this,           &DeviceDiscovererBLE::finished);

    discoveryAgent->setLowEnergyDiscoveryTimeout(10000);
    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

    #ifdef Q_OS_ANDROID
    // This is a trick, in case a robot has already been paired but is not known by the
    // application : the scan agent will not notify us that it exists (well, sometimes it does,
    // but most often it does not...)
    QAndroidJniObject adapter =
            QAndroidJniObject::callStaticObjectMethod("android/bluetooth/BluetoothAdapter",
                                                      "getDefaultAdapter",
                                                      "()Landroid/bluetooth/BluetoothAdapter;");
    QAndroidJniObject pairedDevicesSet = adapter.callObjectMethod("getBondedDevices",
                                                                  "()Ljava/util/Set;");
    jint size = pairedDevicesSet.callMethod<jint>("size");
    if(size > 0)
    {
        QAndroidJniObject iterator = pairedDevicesSet.callObjectMethod("iterator",
                                                                       "()Ljava/util/Iterator;");
        for(int i = 0 ; i < size ; ++i)
        {
            QAndroidJniObject dev = iterator.callObjectMethod("next","()Ljava/lang/Object;");
            QString name = dev.callObjectMethod("getName","()Ljava/lang/String;").toString();
            #warning Do not check based on name...
            if(name == ConstantsCommunication::bluetoothDeviceNameTortax)
            {
                QString addressStr = dev.callObjectMethod("getAddress",
                                                          "()Ljava/lang/String;").toString();
                QBluetoothAddress address(addressStr);
                if(!address.isNull())
                {
                    CommunicationMethod method;
                    method.type = CommunicationType::BluetoothLowEnergy;
                    method.btAddress = address;
                    tryConnectRobot(method, ChannelHandlerMode::SinglePromotable);
                }
                else
                {
                    qWarning() << "Unable to parse existing device address" << addressStr;
                }
            }
        }
    }
#endif
}

void DeviceDiscovererBLE::onDiscoverError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    qWarning() << "Discovery error" << error;
    emit finished();
}

void DeviceDiscovererBLE::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    if(info.name() == ConstantsCommunication::bluetoothDeviceNameTortax)
    {
        auto method = new CommunicationMethod(this, CommunicationType::BluetoothLowEnergy);
        method->setBtAddress(info.address());

        ProductModel::Enum model = ProductModel::Tortax;
        emit deviceDiscovered(model, QString(), ProductModel::getPixmap(model), method);
    }
}
