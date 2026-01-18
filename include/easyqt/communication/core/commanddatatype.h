#pragma once

#include "easyqt/enum.h"

class CommandDataType : public QObject
{
    Q_OBJECT
    Q_ENUMS(Enum)

public:
    enum Enum
    {
        Request,
        Answer
    };

    ENUM_DEBUG()
    ENUM_TOSTRING()
};
