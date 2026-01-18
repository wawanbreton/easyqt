#pragma once

#include "platform/communication/datastreams/abstractdatastreamprovider.h"

class SerialPortServer : public AbstractDataStreamProvider
{
    Q_OBJECT

    public:
        explicit SerialPortServer(quint16 vendorId, quint16 productId, QObject *parent = nullptr);

    private slots:
        void onPortAdded(const QString &port);

        void onPortRemoved(const QString &port);
};
