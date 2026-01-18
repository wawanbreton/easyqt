#include "channelhandlerble.h"

#include "platform/communication/datastreams/blepseudosocket.h"
#include "platform/utils/qt/assertutils.h"


ChannelHandlerBLE::ChannelHandlerBLE(QObject *parent,
                                     const QBluetoothAddress &address,
                                     const ChannelHandlerMode::Enum &mode) :
    AbstractChannelHandler(parent, mode),
    _address(address)
{
}

void ChannelHandlerBLE::connectImpl()
{
    BLEPseudoSocket *socket = new BLEPseudoSocket(this, _address);

    connect(socket, &BLEPseudoSocket::connected,
            this,   &ChannelHandlerBLE::onSocketConnected);
    connect(socket, &BLEPseudoSocket::disconnected,
            this,   &ChannelHandlerBLE::onDisconnected);
    connect(socket, &BLEPseudoSocket::error,
            this,   &ChannelHandlerBLE::onSocketError);
    connect(socket, &BLEPseudoSocket::disconnected,
            socket, &BLEPseudoSocket::deleteLater);
    connect(socket, &BLEPseudoSocket::error,
            socket, &BLEPseudoSocket::deleteLater);

    socket->connectToDevice();
}

void ChannelHandlerBLE::onSocketConnected()
{
    IF_CAST_SENDER(BLEPseudoSocket, socket)
    {
        setConnectedSocket(socket);
    }
}

void ChannelHandlerBLE::onSocketError(QLowEnergyController::Error error)
{
    qWarning() << "BLE socket error :" << error;

    onConnectionFinished(false);
}
