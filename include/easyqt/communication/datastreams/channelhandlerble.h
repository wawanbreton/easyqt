#pragma once

#include "platform/communication/datastreams/abstractchannelhandler.h"

#include <QLowEnergyController>

class ChannelHandlerBLE : public AbstractChannelHandler
{
    Q_OBJECT

    public:
        explicit ChannelHandlerBLE(QObject *parent,
                                   const QBluetoothAddress &address,
                                   const ChannelHandlerMode::Enum &mode);

        virtual void connectImpl() override;

    private:
        void onSocketConnected();

        void onSocketDisconnected();

        void onSocketError(QLowEnergyController::Error error);

    private:
        const QBluetoothAddress _address;
};

