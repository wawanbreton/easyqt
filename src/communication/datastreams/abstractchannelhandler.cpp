#include "abstractchannelhandler.h"

#include "platform/communication/core/robot/abstractrobotinterface.h"
#include "platform/communication/connectedrobotsmanager.h"
#include "platform/communication/robotconfiguration.h"
#include "platform/communication/robotsconfigurationsmanager.h"


AbstractChannelHandler::AbstractChannelHandler(QObject *parent,
                                               const ChannelHandlerMode::Enum &mode) :
    QObject(parent),
    _mode(mode)
{
    auto robotsManager = RobotsConfigurationsManager::access();
    robotsManager->registerAvailableListener(this, &AbstractChannelHandler::onRobotConnected);
}

void AbstractChannelHandler::connectToDevice()
{
    if(_state == ChannelHandlerState::Idle)
    {
        setState(ChannelHandlerState::Connecting);
        connectImpl();
    }
}

void AbstractChannelHandler::setState(const ChannelHandlerState::Enum &state)
{
    if(state != _state)
    {
        _state = state;

        emit stateChanged(_state);

        if(isFinished() && _mode == ChannelHandlerMode::SingleUse)
        {
            emit finished();
        }
    }
}

void AbstractChannelHandler::reconnect()
{
    setState(ChannelHandlerState::Idle);

    QTimer::singleShot(1000, this, &AbstractChannelHandler::connectToDevice);
}

void AbstractChannelHandler::setConnectedSocket(QIODevice *device)
{
    if(_state == ChannelHandlerState::Connecting)
    {
        _connectedDevice = device;

        if(_connectedDevice)
        {
            setState(ChannelHandlerState::Configuring);

            ConnectedRobotsManager::access()->addConnectedSocket(_connectedDevice);
        }
    }
}

void AbstractChannelHandler::forceCloseDevice()
{
    if(_state == ChannelHandlerState::Connected)
    {
        if(_connectedDevice && _connectedDevice->isOpen())
        {
            _connectedDevice->close();
        }
    }
}

void AbstractChannelHandler::onRobotConnected(RobotConfiguration *robot,
                                              const RobotInitResult::Enum &result)
{
    if(_state == ChannelHandlerState::Configuring)
    {
        auto robotDevice = robot->getRobot()->getDevice();
        if(robotDevice && robotDevice == _connectedDevice)
        {
            onConnectionFinished(true);
        }
    }
}

void AbstractChannelHandler::onConnectionFinished(const bool &success)
{
    if(success)
    {
        if(_state == ChannelHandlerState::Configuring)
        {
            if(_mode == ChannelHandlerMode::SinglePromotable)
            {
                _mode = ChannelHandlerMode::AlwaysConnected;
            }

            setState(ChannelHandlerState::Connected);
        }
    }
    else
    {
        if(_state == ChannelHandlerState::Connecting || _state == ChannelHandlerState::Configuring)
        {
            if(_mode == ChannelHandlerMode::SinglePromotable)
            {
                _mode = ChannelHandlerMode::SingleUse;
            }

            if(_mode == ChannelHandlerMode::AlwaysConnected)
            {
                reconnect();
            }
            else
            {
                setState(ChannelHandlerState::Failed);
            }
        }
    }
}

void AbstractChannelHandler::onDisconnected()
{
    if(_mode == ChannelHandlerMode::AlwaysConnected)
    {
        reconnect();
    }
}
