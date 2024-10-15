#pragma once

#include <QString>

#include "easyqt/parser.h"

class QJsonObject;
class QObject;
class QString;

namespace Json
{

enum class WarnIfNotFound
{
    Yes,
    No
};

void mapValuesToObjectProperties(const QJsonObject& json_object, QObject* object);

template<typename Type>
Type loadValue(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    const Type& default_value,
    WarnIfNotFound warn_not_found = WarnIfNotFound::Yes);

template<typename Type>
std::optional<Type> loadValue(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found = WarnIfNotFound::Yes);

}; // namespace Json


template<typename Type>
Type Json::loadValue(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    const Type& default_value,
    WarnIfNotFound warn_not_found)
{
    return loadValue<Type>(json_object, property_name, debug_caller, warn_not_found).value_or(default_value);
}

// Implemented in cpp file, but forward-declared here to avoid multiple definitions
template<>
std::optional<QString> Json::loadValue(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found);

template<typename Type>
std::optional<Type> Json::loadValue(
    const QJsonObject& json_object,
    const QString& property_name,
    const QString& debug_caller,
    WarnIfNotFound warn_not_found)
{
    std::optional<QString> property_str = loadValue<QString>(json_object, property_name, debug_caller, warn_not_found);
    if (property_str.has_value())
    {
        return Parser::parseFromString<Type>(property_str.value(), debug_caller);
    }

    return std::nullopt;
}
