#include "devicediscovererlocalnetwork.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

#include "platform/communication/core/communicationmethod.h"
#include "platform/core/productmodel.h"
#include "platform/constants.h"
#include "platform/utils/io/multicastudpsocket.h"


DeviceDiscovererLocalNetwork::DeviceDiscovererLocalNetwork(QObject *parent) :
    AbstractDeviceDiscoverer(parent),
    _multiCastSocket(new MultiCastUdpSocket(ConstantsCommunication::multiCastGroup,
                                            ConstantsCommunication::multiCastPort,
                                            this))
{
    connect(_multiCastSocket, &MultiCastUdpSocket::datagramReceived,
            this,             &DeviceDiscovererLocalNetwork::onDataGramReceived);

    QJsonObject jsonObject;
    jsonObject["message"] = "discover";
    _broadcastMessage = QJsonDocument(jsonObject).toJson();
}

void DeviceDiscovererLocalNetwork::start()
{
    QTimer *timerSendBroadcast = new QTimer(this);
    connect(timerSendBroadcast, &QTimer::timeout,
            this,               &DeviceDiscovererLocalNetwork::sendBroadcastMessage);
    timerSendBroadcast->start(500);

    QTimer::singleShot(ConstantsCommunication::LocalNetworkDiscoveryTimeout,
                       this,
                       &DeviceDiscovererLocalNetwork::finished);

    sendBroadcastMessage();
}

void DeviceDiscovererLocalNetwork::onDataGramReceived(const QByteArray &data,
                                                      const QHostAddress &address)
{
    QJsonObject jsonObject = QJsonDocument::fromJson(data).object();
    if(jsonObject["message"].toString() == "announce")
    {
        ProductModel::Enum model = ProductModel::fromString(jsonObject["model"].toString());

        auto method = new CommunicationMethod(this, CommunicationType::Network);
        method->setIpAddress(address);
        method->setPort(static_cast<quint16>(jsonObject["port"].toInt()));

        emit deviceDiscovered(model,
                              jsonObject["serialNumber"].toString(),
                              ProductModel::getPixmap(model),
                              method);
    }
}

void DeviceDiscovererLocalNetwork::sendBroadcastMessage()
{
    _multiCastSocket->sendDataGram(_broadcastMessage);
}
