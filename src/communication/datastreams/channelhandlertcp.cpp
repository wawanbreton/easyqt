#include "channelhandlertcp.h"

#include <QTcpSocket>
#include <QTimer>

#include "platform/communication/robotconfiguration.h"
#include "platform/communication/core/davroscommandsqueue.h"
#include "platform/communication/core/robot/abstractarmadeusslaveinterface.h"
#include "platform/utils/qt/assertutils.h"


ChannelHandlerTcp::ChannelHandlerTcp(QObject *parent,
                                     const QHostAddress &ipAddress,
                                     const quint16 port,
                                     const ChannelHandlerMode::Enum &mode) :
    AbstractChannelHandler(parent, mode),
    _address(ipAddress),
    _port(port)
{
}

void ChannelHandlerTcp::connectImpl()
{
    QTcpSocket *socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected,
            this,   &ChannelHandlerTcp::onSocketConnected);
    connect(socket, &QTcpSocket::disconnected,
            socket, &QTcpSocket::deleteLater);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    connect(socket, &QTcpSocket::errorOccurred,
    #else
    connect(socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error),
    #endif
            this,   &ChannelHandlerTcp::onSocketError);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    connect(socket, &QTcpSocket::errorOccurred,
    #else
    connect(socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error),
    #endif
            socket, &QTcpSocket::deleteLater);

    socket->connectToHost(_address, _port);
}

void ChannelHandlerTcp::onRobotConnected(RobotConfiguration *robot,
                                         const RobotInitResult::Enum &result)
{
    #ifdef COMMUNICATION_ARMADEUS_SLAVE
    _armadeusInterface.clear();
    #endif

    auto interface = robot->accessRobot();
    if(interface)
    {
        auto queue = qobject_cast<const DavrosCommandsQueue *>(interface->getQueue());

        // Only DAVROS commands queue is robust enough to handle keep alive requests. With old
        // protocol, it causes some troubles as commands are not acknowledged.
        if(queue)
        {
            #ifdef COMMUNICATION_ARMADEUS_SLAVE
            _armadeusInterface = qobject_cast<AbstractArmadeusSlaveInterface *>(interface);

            if(_armadeusInterface)
            {
                // Some communication devices are very long to automatically disconnect, so we
                // send regular keep-alive commands and close the connection if it fails
                onKeepAliveSuccess();
            }
            #endif
        }
    }

    AbstractChannelHandler::onRobotConnected(robot, result);
}

void ChannelHandlerTcp::onSocketConnected()
{
    IF_CAST_SENDER(QTcpSocket, socket)
    {
        setConnectedSocket(socket);
    }
}

void ChannelHandlerTcp::onSocketDisconnected()
{
    IF_CAST_SENDER(QTcpSocket, socket)
    {
        onDisconnected();

        socket->deleteLater();
    }
}

void ChannelHandlerTcp::onSocketError(QAbstractSocket::SocketError error)
{
    qWarning() << "TCP Error when connecting to robot :" << error;

    onConnectionFinished(false);
}

void ChannelHandlerTcp::sendKeepAlive()
{
    #ifdef COMMUNICATION_ARMADEUS_SLAVE
    if(_armadeusInterface)
    {
        _armadeusInterface->readArmadeusSoftwareVersion(this,
                                                        qBind(&ChannelHandlerTcp::onKeepAliveSuccess),
                                                        qBind(&ChannelHandlerTcp::forceCloseDevice),
                                                        5000);
    }
    #endif
}

void ChannelHandlerTcp::onKeepAliveSuccess()
{
    QTimer::singleShot(1000, this, &ChannelHandlerTcp::sendKeepAlive);
}
