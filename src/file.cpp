#include "easyqt/file.h"

#include <QDebug>
#include <QFile>
#include <QSaveFile>
#include <errno.h>
#include <unistd.h>


namespace easyqt
{

bool File::writeToFile(const QString& file_path, const QString& value, WriteFileMode mode)
{
    return writeToFile(file_path, value.toUtf8(), mode);
}

bool File::writeToFile(const QString& file_path, const QByteArray& data, WriteFileMode mode)
{
    bool result = false;

    QFileDevice* file;
    if (mode == WriteFileMode::Safe)
    {
        file = new QSaveFile(file_path);
    }
    else
    {
        file = new QFile(file_path);
    }

    if (file->open(QIODevice::WriteOnly))
    {
        qint64 written_bytes = file->write(data);

        if (mode == WriteFileMode::Safe)
        {
            result = static_cast<QSaveFile*>(file)->commit();
        }
        else
        {
            result = (written_bytes == data.size());

            if (mode == WriteFileMode::Sync)
            {
                sync(*file);
            }
        }
    }
    else
    {
        qWarning() << "Unable to open file" << file_path << ":" << file->errorString();
    }

    delete file;
    return result;
}

QByteArray File::readFile(const QString& file_path)
{
    QFile file(file_path);
    if (file.open(QIODevice::ReadOnly))
    {
        return file.readAll();
    }
    else
    {
        qWarning() << "Unable to open file" << file_path << ":" << file.errorString();
    }

    return QByteArray();
}

bool File::appendToFile(const QString& file_path, const QString& value, bool log_error)
{
    return appendToFile(file_path, value.toUtf8(), log_error);
}

bool File::appendToFile(const QString& file_path, const QByteArray& data, bool log_error)
{
    QFile file(file_path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        return file.write(data);
    }
    else if (log_error)
    {
        qWarning() << "Unable to open file" << file_path << ":" << file.errorString();
    }

    return false;
}

bool File::move(const QString& src_path, const QString& dst_path, bool very_safe)
{
    if (rename(src_path.toStdString().c_str(), dst_path.toStdString().c_str()) == 0)
    {
        return true;
    }
    else if (! very_safe && (errno == EXDEV || errno == EEXIST))
    {
        if (QFile::exists(dst_path) && ! QFile::remove(dst_path))
        {
            return false;
        }

        return QFile::rename(src_path, dst_path);
    }
    else
    {
        return false;
    }
}

void File::sync(QFileDevice& file)
{
    file.flush();
#ifdef Q_OS_LINUX
    fsync(file.handle());
#endif
}

} // namespace easyqt
