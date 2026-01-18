#pragma once

#include <QObject>

#include <QHostAddress>

#include "platform/core/productmodel.h"
#include "platform/communication/core/communicationmethod.h"

class AbstractDeviceConnector;

class AbstractDeviceDiscoverer : public QObject
{
    Q_OBJECT

    public:
        explicit AbstractDeviceDiscoverer(QObject *parent = nullptr);

        virtual void start() = 0;

    signals:
        /*! @brief Signal emitted when a new device is discovered
         *  @param model The model of discovered device
         *  @param serialNumber The device serial number
         *  @param icon The device icon path
         *  @param method The method to be used to communication with the device
         *  @warning Take care of reparenting the method if you need it, or it will be destroyed
         *           with the discoverer */
        void deviceDiscovered(const ProductModel::Enum &model,
                              const QString &serialNumber,
                              const QString &icon,
                              CommunicationMethod *method);

        void finished();
};
