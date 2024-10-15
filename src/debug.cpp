#include "easyqt/debug.h"

#include <QStringList>


QString Debug::shortMethodName(const QString& function)
{
    QString result = function.mid(0, function.indexOf('('));
    return result.mid(result.lastIndexOf(' ') + 1).toUtf8();
}
