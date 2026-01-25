#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QMetaProperty>
#include <QVariant>
#include <concepts>

#include "easyqt/parser.h"

class QObject;
class QString;

namespace easyqt::Json
{

enum class WarnIfNotFound : quint8
{
    Yes,
    No
};

template<typename Type>
std::optional<Type> loadValue(const QJsonValue& json_value, const QString& debug_caller);

template<typename EnumClass>
std::optional<typename EnumClass::Enum> loadValueEnum(const QJsonValue& json_value, const QString& debug_caller);

template<typename Type>
Type loadValue(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    const Type& default_value,
    WarnIfNotFound warn_not_found = WarnIfNotFound::Yes)
{
    return loadValue<Type>(json_object, property_name, debug_caller, warn_not_found).value_or(default_value);
}

template<typename Type>
std::optional<Type> loadProperty(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found = WarnIfNotFound::Yes);


template<typename Type>
Type loadProperty(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    const Type& default_value,
    WarnIfNotFound warn_not_found = WarnIfNotFound::Yes)
{
    return loadProperty<Type>(json_object, property_name, debug_caller, warn_not_found).value_or(default_value);
}

template<typename EnumClass>
std::optional<typename EnumClass::Enum> loadPropertyEnum(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found = WarnIfNotFound::Yes);

template<typename Type, template<typename> typename ContainerType>
ContainerType<Type> loadPropertyArray(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found = WarnIfNotFound::Yes);

template<typename EnumClass, template<typename> typename ContainerType>
ContainerType<typename EnumClass::Enum> loadPropertyArrayEnum(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found = WarnIfNotFound::Yes);

template<typename Type>
QJsonValue saveValue(const Type& value);

template<typename EnumClass>
QJsonValue saveValueEnum(const typename EnumClass::Enum& value);

template<typename Type, template<typename> typename ContainerType>
QJsonArray saveArray(const ContainerType<Type>& values);

template<typename EnumClass, template<typename> typename ContainerType>
QJsonArray saveArrayEnum(const ContainerType<typename EnumClass::Enum>& values);

}; // namespace easyqt::Json


// Implemented in cpp file, but forward-declared here to avoid multiple definitions
template<>
std::optional<QJsonValue> easyqt::Json::loadProperty(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found);

template<typename Type>
std::optional<Type> easyqt::Json::loadProperty(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found)
{
    const std::optional<QJsonValue> property_value
        = loadProperty<QJsonValue>(json_object, property_name, debug_caller, warn_not_found);

    if (! property_value.has_value())
    {
        return std::nullopt;
    }

    return loadValue<Type>(*property_value, debug_caller);
}

template<>
std::optional<QJsonValue> easyqt::Json::loadValue(const QJsonValue& json_value, const QString& debug_caller);

template<>
std::optional<QJsonArray> easyqt::Json::loadValue(const QJsonValue& json_value, const QString& debug_caller);

template<>
std::optional<QJsonObject> easyqt::Json::loadValue(const QJsonValue& json_value, const QString& debug_caller);

template<typename Type>
std::optional<Type> easyqt::Json::loadValue(const QJsonValue& json_value, const QString& debug_caller)
{
    if (json_value.isString())
    {
        return Parser::parseFromString<Type>(json_value.toString(), debug_caller);
    }

    const QVariant value_variant = json_value.toVariant();
    if (value_variant.canConvert<Type>())
    {
        return value_variant.value<Type>();
    }

    qWarning() << "Could not convert" << json_value << "to a" << typeid(Type).name();
    return std::nullopt;
}

template<typename EnumClass>
std::optional<typename EnumClass::Enum>
    easyqt::Json::loadValueEnum(const QJsonValue& json_value, const QString& debug_caller)
{
    return Parser::parseFromStringEnum<EnumClass>(json_value.toString(), debug_caller);
}

template<typename EnumClass>
std::optional<typename EnumClass::Enum> easyqt::Json::loadPropertyEnum(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found)
{
    std::optional<QString> value_str = loadProperty<QString>(json_object, property_name, debug_caller, warn_not_found);
    if (! value_str.has_value())
    {
        return std::nullopt;
    }

    return Parser::parseFromStringEnum<EnumClass>(*value_str);
}

template<typename Type>
QJsonValue easyqt::Json::saveValue(const Type& value)
{
    if constexpr (std::is_integral_v<Type>)
    {
        return QJsonValue(static_cast<qint64>(value));
    }
    else if constexpr (std::is_constructible_v<QJsonValue, Type>)
    {
        return QJsonValue(value); // Constructor exists, use it
    }
    else
    {
        return QJsonValue(Parser::toString<Type>(value));
    }
}

template<typename EnumClass>
QJsonValue easyqt::Json::saveValueEnum(const typename EnumClass::Enum& value)
{
    return saveValue(Parser::toStringEnum<EnumClass>(value));
}

template<typename Type, template<typename> typename ContainerType>
QJsonArray easyqt::Json::saveArray(const ContainerType<Type>& values)
{
    QJsonArray json_array;
    for (const Type& value : values)
    {
        json_array.append(saveValue(value));
    }
    return json_array;
}

template<typename EnumClass, template<typename> typename ContainerType>
QJsonArray easyqt::Json::saveArrayEnum(const ContainerType<typename EnumClass::Enum>& values)
{
    QJsonArray json_array;
    for (const typename EnumClass::Enum value : values)
    {
        json_array.append(saveValueEnum<EnumClass>(value));
    }
    return json_array;
}

template<typename Type, template<typename> typename ContainerType>
ContainerType<Type> easyqt::Json::loadPropertyArray(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found)
{
    ContainerType<Type> result;

    std::optional<QJsonArray> json_array
        = loadProperty<QJsonArray>(json_object, property_name, debug_caller, warn_not_found);
    if (! json_array.has_value())
    {
        return result;
    }

    for (const QJsonValue& json_value : *json_array)
    {
        const std::optional<Type> value = loadValue<Type>(json_value, debug_caller);
        if (value.has_value())
        {
            result << *value;
        }
    }

    return result;
}

template<typename EnumClass, template<typename> typename ContainerType>
ContainerType<typename EnumClass::Enum> easyqt::Json::loadPropertyArrayEnum(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found)
{
    ContainerType<typename EnumClass::Enum> result;

    std::optional<QJsonArray> json_array
        = loadProperty<QJsonArray>(json_object, property_name, debug_caller, warn_not_found);
    if (! json_array.has_value())
    {
        return result;
    }

    for (const QJsonValue& json_value : *json_array)
    {
        const std::optional<typename EnumClass::Enum> value = loadValueEnum<EnumClass>(json_value, debug_caller);
        if (value.has_value())
        {
            result << *value;
        }
    }

    return result;
}
