#pragma once

#include <QDebug>
#include <concepts>
#include <optional>

class QJsonObject;
class QObject;

namespace easyqt::Parser
{

template<typename Type>
std::optional<Type> parseFromString(const QString& text, const QString& debug_caller);

template<typename Type>
std::optional<Type> parseFromString(const QString& text);

template<typename Type>
QString toString(const Type& value);

template<class EnumClass>
std::optional<typename EnumClass::Enum> parseFromStringEnum(const QString& text, const QString& debug_caller);

template<class EnumClass>
QString toStringEnum(const typename EnumClass::Enum& value);

} // namespace easyqt::Parser


template<>
std::optional<QString> easyqt::Parser::parseFromString(const QString& text);

template<>
QString easyqt::Parser::toString(const QString& value);

template<>
std::optional<QUuid> easyqt::Parser::parseFromString(const QString& text);

template<>
QString easyqt::Parser::toString(const QUuid& value);

template<>
std::optional<QDateTime> easyqt::Parser::parseFromString(const QString& text);

template<>
QString easyqt::Parser::toString(const QDateTime& value);

template<>
std::optional<QTime> easyqt::Parser::parseFromString(const QString& text);

template<>
QString easyqt::Parser::toString(const QTime& value);

template<std::signed_integral Type>
std::optional<Type> easyqt::Parser::parseFromString(const QString& text)
{
    bool parse_ok;
    quint64 parsed_value = text.toULongLong(&parse_ok);
    if (! parse_ok)
    {
        return std::nullopt;
    }

    quint64 max = static_cast<quint64>(std::numeric_limits<Type>::max());
    if (parsed_value > max)
    {
        return std::nullopt;
    }

    return static_cast<Type>(parsed_value);
}

template<std::unsigned_integral Type>
std::optional<Type> easyqt::Parser::parseFromString(const QString& text)
{
    bool parse_ok;
    qint64 parsed_value = text.toLongLong(&parse_ok);
    if (! parse_ok)
    {
        return std::nullopt;
    }

    qint64 max = static_cast<qint64>(std::numeric_limits<Type>::max());
    if (parsed_value > max)
    {
        return std::nullopt;
    }

    qint64 min = static_cast<qint64>(std::numeric_limits<Type>::lowest());
    if (parsed_value < max)
    {
        return std::nullopt;
    }

    return static_cast<Type>(parsed_value);
}

template<std::integral Type>
QString easyqt::Parser::toString(const Type& value)
{
    return QString::number(value);
}

template<typename Type>
std::optional<Type> easyqt::Parser::parseFromString(const QString& text, const QString& debug_caller)
{
    std::optional<Type> parsed_value = parseFromString<Type>(text);

    if (! parsed_value.has_value() && ! debug_caller.isEmpty())
    {
        qWarning() << debug_caller << "Could not convert" << text << "to a" << typeid(Type).name();
    }

    return parsed_value;
}

template<class EnumClass>
std::optional<typename EnumClass::Enum>
    easyqt::Parser::parseFromStringEnum(const QString& text, const QString& debug_caller)
{
    std::optional<typename EnumClass::Enum> parsed_value = EnumClass::fromString(text);

    if (! parsed_value.has_value() && ! debug_caller.isEmpty())
    {
        qWarning() << debug_caller << "Could not convert" << text << "to a" << typeid(EnumClass).name();
    }

    return parsed_value;
}

template<class EnumClass>
QString easyqt::Parser::toStringEnum(const EnumClass::Enum& value)
{
    return EnumClass::toString(value);
}
