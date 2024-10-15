#pragma once

#include <QDebug>
#include <optional>

class QJsonObject;
class QObject;

namespace Parser
{
template<typename Type>
std::optional<Type> parseFromString(const QString& text, const QString& debug_caller);

template<typename Type>
std::optional<Type> parseFromString(const QString& text);
} // namespace Parser


template<typename Type>
std::optional<Type> Parser::parseFromString(const QString& text, const QString& debug_caller)
{
    std::optional<Type> parsed_value = parseFromString<Type>(text);

    if (! parsed_value.has_value() && ! debug_caller.isEmpty())
    {
        qWarning() << debug_caller << "Could not convert" << text << "to a" << typeid(Type).name();
    }

    return parsed_value;
}
