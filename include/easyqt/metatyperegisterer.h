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

template<class T>
class MetaTypeDebugRegisterer
{
public:
    MetaTypeDebugRegisterer()
    {
        QMetaType::registerDebugStreamOperator<T>();
    }
};

} // namespace easyqt
