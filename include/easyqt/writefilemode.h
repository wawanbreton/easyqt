#pragma once

namespace easyqt
{

enum class WriteFileMode
{
    Basic, // Just write data to file, do nothing specific
    Sync, // Write data to file, then sync storage device
    Safe, // Write data to file in a way that the file can't be corrupted
};

}
