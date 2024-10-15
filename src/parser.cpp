#include "easyqt/parser.h"

#include <QUuid>


template<>
std::optional<QUuid> Parser::parseFromString(const QString& text)
{
    QUuid result = QUuid::fromString(text);
    return result.isNull() ? std::nullopt : std::optional<QUuid>(result);
}
