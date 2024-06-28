#pragma once

#include "easyqt_global.h"
#include "enum.h"

namespace easyqt
{

class EASYQT_EXPORT ResourceType : public QObject
{
    Q_OBJECT
    Q_ENUMS(Enum)

public:
    enum Enum
    {
        Icon,
        Shader,
    };

    ENUM_DEBUG()
};

} // namespace easyqt

Q_DECLARE_METATYPE(easyqt::ResourceType::Enum)
