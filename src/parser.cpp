#include "easyqt/parser.h"

#include <QDateTime>
#include <QUuid>


template<>
std::optional<QString> easyqt::Parser::parseFromString(const QString& text)
{
    return text;
}

template<>
QString easyqt::Parser::toString(const QString& value)
{
    return value;
}

template<>
std::optional<QUuid> easyqt::Parser::parseFromString(const QString& text)
{
    const QUuid result = QUuid::fromString(text);
    return result.isNull() ? std::nullopt : std::optional<QUuid>(result);
}

template<>
QString easyqt::Parser::toString(const QUuid& value)
{
    return value.toString(QUuid::StringFormat::WithoutBraces);
}

template<>
std::optional<QDateTime> easyqt::Parser::parseFromString(const QString& text)
{
    QDateTime result = QDateTime::fromString(text, "yyyy-MM-dd hh:mm");
    if (result.isValid())
    {
        result.setTimeSpec(Qt::UTC);
        return result;
    }

    return std::nullopt;
}

template<>
QString easyqt::Parser::toString(const QDateTime& value)
{
    return value.toUTC().toString("yyyy-MM-dd hh:mm");
}

template<>
std::optional<QTime> easyqt::Parser::parseFromString(const QString& text)
{
    QTime result = QTime::fromString(text, "HH:mm");
    return result.isValid() ? std::make_optional(result) : std::nullopt;
}

template<>
QString easyqt::Parser::toString(const QTime& value)
{
    return value.toString("HH:mm");
}
