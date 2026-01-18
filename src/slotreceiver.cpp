#include "slotreceiver.h"

#include <QHash>
#include <QTimer>


SlotReceiver::SlotReceiver(QObject *receiver, const QString &slot) :
    _receiver(receiver),
    _slot(slot)
{
}

void SlotReceiver::connect(const QObject *emitter, const QString &signal, Qt::ConnectionType type) const
{
    if(!isNull() && emitter && !signal.isEmpty())
    {
        QObject::connect(emitter, signal.toUtf8().data(),
                         _receiver, _slot.toUtf8().data(),
                         type);
    }
}

void SlotReceiver::callAsap() const
{
    if(!isNull())
    {
        // Can't use QTimer::singleShot in this context
        QTimer *timer = new QTimer();
        connect(timer, SIGNAL(timeout()));
        QObject::connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
        timer->start(0);
    }
}

bool SlotReceiver::isNull() const
{
    return !_receiver || _slot.isEmpty();
}

bool SlotReceiver::operator==(const SlotReceiver &other) const
{
    return other._receiver == _receiver && other._slot == _slot;
}

uint qHash(const SlotReceiver &slotReceiver)
{
    return qHash(qMakePair(slotReceiver._receiver, slotReceiver._slot));
}
