#pragma once

#include <QObject>

namespace easyqt
{

template<class T>
bool is_instance(const QObject* object)
{
    return qobject_cast<const T*>(object) != nullptr;
}

} // namespace easyqt

#define IF_CAST_SENDER(Class, variable) \
    auto variable = qobject_cast<Class*>(sender()); \
    if (Q_UNLIKELY(! variable)) \
    { \
        qCritical() << "Sender is not an instance of" << Class::staticMetaObject.className() << sender(); \
    } \
    else

#define IF_CAST_OBJECT(Class, variable, object) \
    auto variable = qobject_cast<Class*>(object); \
    if (Q_UNLIKELY(! variable)) \
    { \
        qCritical() << "Object is not an instance of" << Class::staticMetaObject.className() << object; \
    } \
    else

#define IF_CONST_CAST_OBJECT(Class, variable, object) \
    auto variable = qobject_cast<const Class*>(object); \
    if (Q_UNLIKELY(! variable)) \
    { \
        qCritical() << "Object is not an instance of" << Class::staticMetaObject.className() << object; \
    } \
    else

#define IF_CHECK_SENDER() \
    if (Q_UNLIKELY(! sender())) \
    { \
        qCritical() << "There is no sender"; \
    } \
    else

#define IF_CAST_SHARED_PTR(Class, variable, pointer) \
    auto variable = std::dynamic_pointer_cast<Class>(pointer); \
    if (Q_UNLIKELY(! variable)) \
    { \
        qCritical() << "Object is not an instance of" << typeid(Class).name(); \
    } \
    else
