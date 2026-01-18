#include "serialportserver.h"

#include <QDebug>
#include <QFile>

#include "platform/utils/io/serialport.h"
#include "platform/utils/io/serialportscanner.h"


SerialPortServer::SerialPortServer(quint16 vendorId, quint16 productId, QObject *parent) :
    AbstractDataStreamProvider(parent)
{
    SerialPortScanner *scanner = new SerialPortScanner(this);

    scanner->setIdFilter(vendorId, productId);

    connect(scanner, &SerialPortScanner::newSerialPort,
            this,    &SerialPortServer::onPortAdded);
    connect(scanner, &SerialPortScanner::portRemoved,
            this,    &SerialPortServer::onPortRemoved);
}

void SerialPortServer::onPortAdded(const QString &port)
{
    SerialPort *device = new SerialPort(port, this);
    if(device->open(QIODevice::ReadWrite))
    {
        addStream(device, device->portName());
    }
    else
    {
        qWarning() << "Unable to open port" << port << ":" << device->errorString();
        delete device;
    }
}

void SerialPortServer::onPortRemoved(const QString &port)
{
    for(QIODevice *stream : getStreams())
    {
        SerialPort *serialPort = qobject_cast<SerialPort *>(stream);
        if(serialPort && port.endsWith(serialPort->portName()))
        {
            removeDevice(stream);
            break;
        }
    }
}
