#pragma once

#include <QMetaType>

namespace easyqt
{

template<class T>
class MetaTypeRegisterer
{
public:
    MetaTypeRegisterer()
    {
        qRegisterMetaType<T>();
    }
};

} // namespace easyqt
