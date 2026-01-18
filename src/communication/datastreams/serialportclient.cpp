#include "serialportclient.h"

#include <QDebug>
#include <QFileInfo>
#include <QTimer>

#include "platform/utils/io/file.h"
#include "platform/utils/io/serialport.h"
#include "platform/utils/io/serialportscanner.h"


SerialPortClient::SerialPortClient(const QString &filePath,
                                   const QString &sysStateFilePath,
                                   QObject *parent) :
    AbstractDataStreamProvider(parent),
    _filePath(filePath),
    _sysStateFilePath(sysStateFilePath),
    _timerPoll(new QTimer(this))
{
    connect(_timerPoll, &QTimer::timeout, this, &SerialPortClient::lookForConnection);
    _timerPoll->setInterval(100);
    _timerPoll->start();
}

void SerialPortClient::lookForConnection()
{
    bool connected = !getStreams().isEmpty();
    bool newConnected = (File::readFile(_sysStateFilePath) == "configured\n");

    if(newConnected != connected)
    {
        if(newConnected)
        {
            _timerPoll->stop();
            QTimer::singleShot(150, this, &SerialPortClient::openSerialPort);
        }
        else
        {
            removeDevice(getStreams().first());
        }
    }
}

void SerialPortClient::openSerialPort()
{
    SerialPort *port = new SerialPort(_filePath, this);
    if(port->open(QIODevice::ReadWrite))
    {
        addStream(port, QFileInfo(_filePath).fileName());
    }
    else
    {
        delete port;
    }

    _timerPoll->start();
}
