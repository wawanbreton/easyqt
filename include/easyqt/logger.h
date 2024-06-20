#pragma once

#include <QObject>

#include "logoutput.h"
#include "logpart.h"
#include "singleton.h"

namespace easyqt
{

/*! @brief Receives the output of standard Qt messages and writes them to standard syslog
 *  @note Don't forget to define QT_MESSAGELOGCONTEXT in the project file to retrieve function
 *        contexts */
class Logger : public QObject
{
    Q_OBJECT
    SINGLETON(Logger)

private:
    static void handleOutput(QtMsgType type, const QMessageLogContext& context, const QString& message);

private:
    struct ConfiguredLogOutput
    {
        LogOutput output;
        LogPart::Flags parts;
    };

private:
    static std::list<ConfiguredLogOutput> outputs_;
    static std::string app_name_;
};

} // namespace easyqt
