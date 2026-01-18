#pragma once

#include "platform/communication/datastreams/abstractchannelhandler.h"

#include <QAbstractSocket>
#include <QPointer>

class AbstractArmadeusSlaveInterface;

class ChannelHandlerTcp : public AbstractChannelHandler
{
    Q_OBJECT

    public:
        explicit ChannelHandlerTcp(QObject *parent,
                                   const QHostAddress &ipAddress,
                                   const quint16 port,
                                   const ChannelHandlerMode::Enum &mode);

    protected:
        virtual void connectImpl() override;

        virtual void onRobotConnected(RobotConfiguration *robot,
                                      const RobotInitResult::Enum &result) override;

    private:
        void onSocketConnected();

        void onSocketDisconnected();

        void onSocketError(QAbstractSocket::SocketError error);

        void sendKeepAlive();

        void onKeepAliveSuccess();

    private:
        const QHostAddress _address;
        const quint16 _port;
        #ifdef COMMUNICATION_ARMADEUS_SLAVE
        QPointer<AbstractArmadeusSlaveInterface> _armadeusInterface;
        #endif
};

