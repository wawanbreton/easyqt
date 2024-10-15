#include "easyqt/json.h"

#include <QJsonObject>
#include <QMetaProperty>


void Json::mapValuesToObjectProperties(const QJsonObject& json_object, QObject* object)
{
    const QMetaObject* meta_object = object->metaObject();

    for (auto iterator = json_object.constBegin(); iterator != json_object.constEnd(); ++iterator)
    {
        const QString json_value_name = iterator.key();

        const int property_index = meta_object->indexOfProperty(json_value_name.toLocal8Bit().data());
        if (property_index >= 0)
        {
            QMetaProperty property = meta_object->property(property_index);

#warning this should be done in the parser, but how to give arguments ?
            if (property.typeId() == qMetaTypeId<QDateTime>())
            {
                QDateTime date_time = QDateTime::fromString(iterator.value().toString(), "yyyy-MM-dd hh:mm");
                date_time.setTimeSpec(Qt::UTC);
                property.write(object, date_time);
            }
            else
            {
                property.write(object, iterator.value().toVariant());
            }
        }
    }
}

template<>
std::optional<QString> Json::loadValue(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found)
{
    auto iterator = json_object.constFind(property_name);
    if (iterator != json_object.constEnd())
    {
        const QJsonValue& json_value = iterator.value();
        if (json_value.isString())
        {
            return json_value.toString();
        }
        else
        {
            qWarning() << debug_caller << "Value for attribute" << property_name << "is not a string as expected";
        }
    }
    else if (warn_not_found == WarnIfNotFound::Yes)
    {
        qWarning() << debug_caller << "Could not find the required attribute" << property_name;
    }

    return std::nullopt;
}
