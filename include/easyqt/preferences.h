#pragma once

#include <QMetaType>
#include <QObject>
#include <QVariant>

class QSettings;

namespace easyqt
{

class Preferences : public QObject
{
    Q_OBJECT

signals:
    void valueChanged(const int entry);

protected:
    explicit Preferences(QObject* parent = nullptr);

    void registerPreference(
        int entry,
        const QMetaType::Type type,
        const QVariant& default_value,
        const QString& storage_key);

    template<class ValueType>
    ValueType getTypedValue(const int entry) const;

    QVariant getValue(const int entry) const;

    void setValue(const int entry, const QVariant& value);

private:
    struct RegisteredEntry
    {
        QString storage_key;
        QMetaType::Type type;
        QVariant default_value;
    };

private:
    QSettings* settings_;
    QHash<int, RegisteredEntry> registered_entries_;
};


// Template methods definition
template<class ValueType>
ValueType Preferences::getTypedValue(const int entry) const
{
    QVariant value = getValue(entry);
    if (! value.isNull())
    {
        if (value.typeId() == qMetaTypeId<ValueType>())
        {
            return value.value<ValueType>();
        }
        else
        {
            qWarning() << "Value for preference" << entry << "is not a" << QMetaType(qMetaTypeId<ValueType>()).name();
        }
    }

    return ValueType();
}

} // namespace easyqt
