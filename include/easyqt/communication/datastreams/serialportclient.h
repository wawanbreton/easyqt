#pragma once

#include <QTimer>

#include "platform/communication/datastreams/abstractdatastreamprovider.h"

class SerialPortClient : public AbstractDataStreamProvider
{
    Q_OBJECT

    public:
        explicit SerialPortClient(const QString &filePath,
                                  const QString &sysStateFilePath,
                                  QObject *parent = nullptr);

    private:
        void lookForConnection();

        void openSerialPort();

        void onSysFileData(const QByteArray &data);

    private:
        const QString _filePath;
        const QString _sysStateFilePath;
        QTimer *_timerPoll{nullptr};
};
