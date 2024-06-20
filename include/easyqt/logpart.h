#pragma once

#include <QFlag>

namespace easyqt::LogPart
{

enum Enum
{
    Level = 0x1,
    Date = 0x2,
    Message = 0x4
};

Q_DECLARE_FLAGS(Flags, Enum)
Q_DECLARE_OPERATORS_FOR_FLAGS(Flags)

} // namespace easyqt::LogPart
