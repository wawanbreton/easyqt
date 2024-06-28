#include "easyqt/preferences.h"

#include <QCoreApplication>
#include <QSettings>

#include "easyqt/datastorage.h"


namespace easyqt
{

Preferences::Preferences(QObject* parent)
    : QObject{ parent }
    , settings_(new QSettings(DataStorage::get()->configFile("preferences.cfg"), QSettings::IniFormat, this))
{
    qInfo() << "Storing preferences in file" << settings_->fileName();
}

void Preferences::registerPreference(
    int entry,
    const QMetaType::Type type,
    const QVariant& default_value,
    const QString& storage_key)
{
    if (registered_entries_.contains(entry))
    {
        qCritical() << "Entry" << entry << "is already registered, this shouldn't be done multiple times";
    }

    registered_entries_.insert(entry, { storage_key, type, default_value });
}

QVariant Preferences::getValue(const int entry) const
{
    QVariant result;

    auto iterator = registered_entries_.find(entry);
    if (iterator != registered_entries_.end())
    {
        const RegisteredEntry& registered_entry = iterator.value();
        QVariant stored_value = settings_->value(registered_entry.storage_key);
        if (! stored_value.isNull())
        {
            if (stored_value.typeId() == registered_entry.type)
            {
                result = stored_value;
            }
            else
            {
                qWarning() << "Expected value of type" << registered_entry.type << "for key"
                           << registered_entry.storage_key << "but got type" << stored_value.typeId();
            }
        }

        if (result.isNull())
        {
            result = registered_entry.default_value;
        }
    }
    else
    {
        qCritical() << "Entry" << entry
                    << "is not registered: if you are a developer, please register it in the constructor above";
    }

    return result;
}

void Preferences::setValue(const int entry, const QVariant& value)
{
    auto iterator = registered_entries_.find(entry);
    if (iterator != registered_entries_.end())
    {
        const RegisteredEntry& registered_entry = iterator.value();
        if (value.typeId() == registered_entry.type)
        {
            if (value != settings_->value(registered_entry.storage_key))
            {
                settings_->setValue(registered_entry.storage_key, value);
                emit valueChanged(entry);
            }
        }
        else
        {
            qWarning() << "Expected value of type" << registered_entry.type << "for key" << registered_entry.storage_key
                       << "but got type" << value.typeId();
        }
    }
    else
    {
        qCritical() << "Entry" << entry
                    << "is not registered: if you are a developer, please register it in the constructor above";
    }
}

} // namespace easyqt
