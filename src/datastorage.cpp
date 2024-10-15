#include "easyqt/datastorage.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>


namespace easyqt
{

SINGLETON_IMPL(DataStorage)
SINGLETON_DESTRUCTOR_IMPL(DataStorage)

DataStorage::DataStorage(QObject* parent)
    : QObject(parent)
    , tmp_dir_(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
    , config_dir_(QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
                      .absoluteFilePath(qApp->applicationVersion()))
    , resources_dir_(QDir().absoluteFilePath("resources"))
{
    checkDir(tmp_dir_);

    if (config_dir_ != tmp_dir_)
    {
        checkDir(config_dir_);
    }

    qInfo() << "Looking for resources in" << resources_dir_;
}

QDir DataStorage::tmpDir()
{
    return QDir(DataStorage::get()->tmp_dir_);
}

QString DataStorage::tmpFile(const QString& file_name)
{
    return tmpDir().absoluteFilePath(file_name);
}

QDir DataStorage::configDir()
{
    return QDir(DataStorage::get()->config_dir_);
}

QString DataStorage::configFile(const QString& file_name)
{
    return configDir().absoluteFilePath(file_name);
}

QDir DataStorage::resourcesDir(const std::optional<ResourceType::Enum> resource_type)
{
    QDir dir = QDir(DataStorage::get()->resources_dir_);
    if (resource_type)
    {
        QString sub_directory_name;

        switch (resource_type.value())
        {
            case ResourceType::Icon:
                sub_directory_name = "icons";
                break;
            case ResourceType::Shader:
                sub_directory_name = "shaders";
                break;
            case ResourceType::Font:
                sub_directory_name = "fonts";
                break;
        }

        dir = QDir(dir.absoluteFilePath(sub_directory_name));
    }

    return dir;
}

QString DataStorage::resourceFile(const QString& file_name, const std::optional<ResourceType::Enum> resource_type)
{
    return resourcesDir(resource_type).absoluteFilePath(file_name);
}

QString DataStorage::findResource(
    const QString& file_base_name,
    const ResourceType::Enum resource_type,
    const QStringList& allowed_extensions)
{
    QDir resources_dir = resourcesDir(resource_type);
    for (const QFileInfo& file : resources_dir.entryInfoList(QDir::Files))
    {
        if (file.baseName() == file_base_name
            && (allowed_extensions.empty() || allowed_extensions.contains(file.suffix())))
        {
            return file.absoluteFilePath();
        }
    }

    qWarning() << "No resource file found for name" << file_base_name << "in" << resource_type;
    return QString();
}

void DataStorage::checkDir(const QString& dirPath) const
{
    QDir dir(dirPath);
    if (! dir.exists())
    {
        dir.mkpath(dirPath);
    }

    if (dir.exists())
    {
        qInfo() << "Directory" << dir.absolutePath() << "is ready for writing";
    }
    else
    {
        qWarning() << "Directory" << dir.absolutePath() << "could not be created";
    }
}

} // namespace easyqt
