#pragma once

#include <QObject>

#include "platform/communication/core/communicationmethod.h"
#include "platform/communication/datastreams/channelhandlermode.h"
#include "platform/communication/datastreams/channelhandlerstate.h"
#include "platform/communication/robotinitresult.h"

class RobotConfiguration;

/*! @brief This class represents an object in charge of a specific communication channel in the long
 *         term, for example a TCP connection on address 192.168.0.58, or a BLE connection on
 *         CC:DD:EE:FF:AA:BB. It is in charge of initializing the connection, keeping it alive if
 *         necessary, and reconnecting it. Some options are given at initialization to actually make
 *         it a long-term object or a temporary object. */
class AbstractChannelHandler : public QObject
{
    Q_OBJECT

    public:
        explicit AbstractChannelHandler(QObject *parent, const ChannelHandlerMode::Enum &mode);

        void connectToDevice();

        bool isFinished() const
        { return _state == ChannelHandlerState::Connected || _state == ChannelHandlerState::Failed; }

        bool isConnected() const { return _state == ChannelHandlerState::Connected; }

    signals:
        void stateChanged(const ChannelHandlerState::Enum &state);

        void finished();

    protected:
        void onConnectionFinished(const bool &success);

        void onDisconnected();

        void setConnectedSocket(QIODevice *device);

        void forceCloseDevice();

        virtual void connectImpl() = 0;

        virtual void onRobotConnected(RobotConfiguration *robot,
                                      const RobotInitResult::Enum &result);

    private:
        void setState(const ChannelHandlerState::Enum &state);

        void reconnect();

    private:
        ChannelHandlerMode::Enum _mode;
        ChannelHandlerState::Enum _state{ChannelHandlerState::Idle};
        QIODevice *_connectedDevice{nullptr};
};

