#include "easyqt/logger.h"

#include <iostream>
#ifdef Q_OS_LINUX
#include <syslog.h>
#endif

#include <QCoreApplication>
#include <QDateTime>
#include <QLibraryInfo>

#include "easyqt/datastorage.h"
#include "easyqt/file.h"

namespace easyqt
{

SINGLETON_IMPL(Logger)
SINGLETON_DESTRUCTOR_IMPL(Logger)

std::list<Logger::ConfiguredLogOutput> Logger::outputs_;
std::string Logger::app_name_;

Logger::Logger(QObject* parent)
    : QObject(parent)
{
    QString appName = QCoreApplication::applicationName();
    app_name_ = appName.toStdString();

#ifndef Q_OS_ANDROID
    outputs_.push_back({ LogOutput::Console, LogPart::Level | LogPart::Date | LogPart::Message });
#endif

#ifdef Q_OS_LINUX
    outputs_.push_back({ LogOutput::Syslog, LogPart::Date | LogPart::Message });
#else
    _outputs.push_back({ LogOutput::File, LogPart::Level | LogPart::Date | LogPart::Message });
#pragma message("warning Remove old files")
#endif

    qInstallMessageHandler(handleOutput);

    qInfo() << "Starting application" << appName << "version" << QCoreApplication::applicationVersion();
}

void Logger::handleOutput(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    QString levelString;
    switch (type)
    {
        case QtDebugMsg:
            levelString = "DEBUG";
            break;
        case QtWarningMsg:
            levelString = "WARNING";
            break;
        case QtInfoMsg:
            levelString = "INFO";
            break;
        case QtCriticalMsg:
            levelString = "CRITICAL";
            break;
        case QtFatalMsg:
            levelString = "FATAL";
            break;
    }

    levelString = QString("[%1]").arg(levelString).leftJustified(11, ' ');

    QString dateString = QDateTime::currentDateTime().toString("[hh:mm:ss:zzz]");

    QString methodName = QString::fromUtf8(context.function);
    methodName = methodName.mid(0, methodName.indexOf('('));
    methodName = methodName.mid(methodName.lastIndexOf(' ') + 1);
    const QString messageString = QString("%1 || %2").arg(methodName, message);

    for (const ConfiguredLogOutput& output : outputs_)
    {
        QStringList stringParts;
        if (output.parts.testFlag(LogPart::Level))
        {
            stringParts << levelString;
        }
        if (output.parts.testFlag(LogPart::Date))
        {
            stringParts << dateString;
        }
        if (output.parts.testFlag(LogPart::Message))
        {
            stringParts << messageString;
        }
        QString fullString = stringParts.join(' ');

        switch (output.output)
        {
            case LogOutput::Console:
                std::cout << fullString.toStdString() << std::endl;
                break;

            case LogOutput::File:
            {
                QString fileName = QString("%1.log").arg(QDate::currentDate().toString("yyyy-MM-dd"));
                QString filePath = DataStorage::configFile(fileName);
                File::appendToFile(filePath, QString("%1\n").arg(fullString).toUtf8(), false);
                break;
            }

            case LogOutput::Syslog:
            {
#ifdef Q_OS_LINUX
                int syslogLevel = -1;
                switch (type)
                {
                    case QtDebugMsg:
                        syslogLevel = LOG_DEBUG;
                        break;
                    case QtWarningMsg:
                        syslogLevel = LOG_WARNING;
                        break;
                    case QtInfoMsg:
                        syslogLevel = LOG_INFO;
                        break;
                    case QtCriticalMsg:
                        syslogLevel = LOG_CRIT;
                        break;
                    case QtFatalMsg:
                        syslogLevel = LOG_ALERT;
                        break;
                }

                syslog(syslogLevel, "%s", fullString.toLocal8Bit().constData());
#else
                qCritical() << "Syslog not available for this platform";
#endif
                break;
            }
        }
    }
}

} // namespace easyqt
