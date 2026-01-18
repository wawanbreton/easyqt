#pragma once

#include <QObject>

#define qBind(fun) std::bind(fun, this)
#define qBind1(fun) std::bind(fun, this, std::placeholders::_1)
#define qBind2(fun) std::bind(fun, this, std::placeholders::_1, std::placeholders::_2)

class SlotReceiver
{
    public:
        SlotReceiver(QObject *receiver = nullptr, const QString &slot = QString());

        void connect(const QObject *emitter,
                     const QString &signal,
                     Qt::ConnectionType type = Qt::AutoConnection) const;

        void callAsap() const;

        bool isNull() const;

        bool operator==(const SlotReceiver &other) const;

        friend quint32 qHash(const SlotReceiver &slotReceiver);

    private:
        QObject *_receiver{nullptr};
        QString _slot;
};
