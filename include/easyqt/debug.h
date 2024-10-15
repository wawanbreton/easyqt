#pragma once

#include <QDebug>
#include <QString>

#define __METHOD__ Debug::shortMethodName(__PRETTY_FUNCTION__)

namespace Debug
{
QString shortMethodName(const QString& function);
}
