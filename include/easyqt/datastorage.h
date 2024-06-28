#pragma once

#include <QDir>

#include "easyqt_global.h"
#include "resourcetype.h"
#include "singleton.h"

namespace easyqt
{

/*! @brief Basic manager which handles the various locations to read/store files
 * according to their type and to the actual platform */
class EASYQT_EXPORT DataStorage : public QObject
{
    Q_OBJECT
    SINGLETON(DataStorage)

public:
    static QDir tmpDir();

    static QString tmpFile(const QString& file_name);

    static QDir configDir();

    static QString configFile(const QString& file_name);

    static QDir resourcesDir(const std::optional<ResourceType::Enum> resource_type = std::nullopt);

    static QString
        resourceFile(const QString& file_name, const std::optional<ResourceType::Enum> resource_type = std::nullopt);

    Q_INVOKABLE static QString findResource(
        const QString& file_base_name,
        const ResourceType::Enum resource_type,
        const QStringList& allowed_extensions = QStringList());

private:
    void checkDir(const QString& dirPath) const;

private:
    const QString tmp_dir_;
    const QString config_dir_;
    const QString resources_dir_;
};

} // namespace easyqt
