#include "tcpserver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include "platform/constants.h"
#include "platform/utils/io/multicastudpsocket.h"


TcpServer::TcpServer(quint16 port, QObject *parent) :
    AbstractDataStreamProvider(parent),
    _server(new QTcpServer(this)),
    _multiCastSocket(new MultiCastUdpSocket(ConstantsCommunication::multiCastGroup,
                                            ConstantsCommunication::multiCastPort,
                                            this))
{
    if(_server->listen(QHostAddress::Any, port))
    {
        qInfo() << "Now listening on port" << port;
        connect(_server, &QTcpServer::newConnection, this, &TcpServer::onNewTcpConnection);
    }
    else
    {
        qWarning() << "Unable to listen on port" << port;
    }

    connect(_multiCastSocket, &MultiCastUdpSocket::datagramReceived,
            this,             &TcpServer::onMultiCastSocketDataGramReceived);
}

void TcpServer::onNewTcpConnection()
{
    while(_server->hasPendingConnections())
    {
        QTcpSocket *socket = _server->nextPendingConnection();

        connect(socket, &QTcpSocket::disconnected, this, &TcpServer::onSocketDisconnected);

        addStream(socket, socket->peerAddress().toString());
    }
}

void TcpServer::onSocketDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if(socket)
    {
        removeDevice(socket);
    }
    else
    {
        qCritical() << "Sender is not a QTcpSocket";
    }
}

void TcpServer::onMultiCastSocketDataGramReceived(const QByteArray &data)
{
    if(QJsonDocument::fromJson(data).object()["message"].toString() == "discover")
    {
        QString model;
        QString serialNumber;

        emit discoverRequest(model, serialNumber);

        if(!model.isEmpty() && !serialNumber.isEmpty())
        {
            QJsonObject jsonObject;
            jsonObject["message"] = "announce";
            jsonObject["model"] = model;
            jsonObject["serialNumber"] = serialNumber;
            jsonObject["port"] = _server->serverPort();

            _multiCastSocket->sendDataGram(QJsonDocument(jsonObject).toJson());
        }
    }
}
