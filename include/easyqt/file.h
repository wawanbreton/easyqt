#pragma once

#include <QString>

#include "writefilemode.h"

class QFileDevice;

namespace easyqt::File
{
bool writeToFile(const QString& file_path, const QString& value, WriteFileMode mode);

bool writeToFile(const QString& file_path, const QByteArray& data, WriteFileMode mode);

bool appendToFile(const QString& file_path, const QString& value, bool log_error = true);

bool appendToFile(const QString& file_path, const QByteArray& data, bool log_error = true);

QByteArray readFile(const QString& file_path);

bool move(const QString& src_path, const QString& dst_path, bool very_safe);

void sync(QFileDevice& file);
} // namespace easyqt::File
