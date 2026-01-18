#pragma once

#include "platform/communication/datastreams/abstractdatastreamprovider.h"

#include <QTcpServer>

class MultiCastUdpSocket;

class TcpServer : public AbstractDataStreamProvider
{
    Q_OBJECT

    public:
        explicit TcpServer(quint16 port, QObject *parent = nullptr);

    signals:
        void discoverRequest(QString &model, QString &serialNumber);

    private:
        void onNewTcpConnection();

        void onSocketDisconnected();

        void onMultiCastSocketDataGramReceived(const QByteArray &data);

    private:
        QTcpServer *_server{nullptr};
        QList<QTcpSocket *> _sockets;
        MultiCastUdpSocket *_multiCastSocket{nullptr};
};
