#include "easyqt/json.h"

#include <QJsonArray>
#include <QMetaProperty>


template<>
std::optional<QJsonValue> easyqt::Json::loadProperty(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found)
{
    auto iterator = json_object.constFind(property_name);
    if (iterator == json_object.constEnd())
    {
        if (warn_not_found == WarnIfNotFound::Yes)
        {
            qWarning() << debug_caller << "Could not find the required property" << property_name;
        }
        return std::nullopt;
    }

    return iterator.value();
}

template<>
std::optional<QJsonArray> easyqt::Json::loadValue(const QJsonValue& json_value, const QString& debug_caller)
{
    if (! json_value.isArray())
    {
        qWarning() << debug_caller << "Value" << json_value << "is not an array as expected";
        return std::nullopt;
    }

    return json_value.toArray();
}

template<>
std::optional<QJsonObject> easyqt::Json::loadValue(const QJsonValue& json_value, const QString& debug_caller)
{
    if (! json_value.isObject())
    {
        qWarning() << debug_caller << "Value" << json_value << "is not an object as expected";
        return std::nullopt;
    }

    return json_value.toObject();
}
