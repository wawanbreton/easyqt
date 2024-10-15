#pragma once

#include <QDebug>

// clang-format off

#define SINGLETON(Class) \
public: \
    static void init(QObject *parent); \
\
    static void destroy(); \
\
    static const Class *get() { return instance_; } \
\
    static Class *access() { return instance_; } \
\
    virtual ~Class(); \
\
protected: \
    explicit Class(QObject *parent = nullptr); \
\
private: \
    static Class *instance_;

#define SINGLETON_WITH_1_PARAM(Class, param) \
public: \
    static void init(QObject *parent, param); \
\
    static const Class *get() { return instance_; } \
\
    static Class *access() { return instance_; } \
\
    virtual ~Class(); \
\
private: \
    explicit Class(QObject *parent, param); \
\
private: \
    static Class *instance_;

#define SINGLETON_WITH_2_PARAMS(Class, param1, param2) \
public: \
    static void init(QObject *parent, param1, param2); \
\
    static const Class *get() { return instance_; } \
\
    static Class *access() { return instance_; } \
\
    virtual ~Class(); \
\
private: \
    explicit Class(QObject *parent, param1, param2); \
\
private: \
    static Class *instance_;

#define SINGLETON_INHERITABLE(Class) \
public: \
    virtual ~Class(); \
\
    static const Class* get() { return instance_; } \
\
    static Class* access() { return instance_; } \
\
protected: \
    explicit Class(QObject *parent = nullptr); \
\
    static void init(Class* instance); \
\
private: \
    static Class* instance_;

#define SINGLETON_IMPL_COMMON(Class) \
Class *Class::instance_ = nullptr; \

#define SINGLETON_INIT(Class) \
void Class::init(QObject *parent) \
{ \
    if(!instance_) \
    { \
        qInfo() << "Init start" << Class::staticMetaObject.className(); \
        instance_ = new Class(parent); \
        qInfo() << "Init done" << Class::staticMetaObject.className(); \
    } \
    else \
    { \
        qCritical() << "There is already a Class instance"; \
    } \
} \

#define SINGLETON_DESTROY(Class) \
void Class::destroy() \
{ \
     if(instance_) \
    { \
         delete instance_; \
         instance_ = nullptr; \
    } \
}

#define SINGLETON_IMPL(Class) \
SINGLETON_IMPL_COMMON(Class) \
\
SINGLETON_INIT(Class) \
\
SINGLETON_DESTROY(Class)

#define SINGLETON_IMPL_WITH_1_PARAM(Class, param, givenParam) \
SINGLETON_IMPL_COMMON(Class) \
\
void Class::init(QObject *parent, param) \
{ \
    if(!instance_) \
    { \
        qInfo() << "Init start" << Class::staticMetaObject.className(); \
        instance_ = new Class(parent, givenParam); \
        qInfo() << "Init done" << Class::staticMetaObject.className(); \
    } \
    else \
    { \
        qCritical() << "There is already a Class instance"; \
    } \
}

#define SINGLETON_IMPL_WITH_2_PARAMS(Class, param1, param2, givenParam1, givenParam2) \
SINGLETON_IMPL_COMMON(Class) \
\
void Class::init(QObject *parent), param1, param2 \
{ \
    if(!instance_) \
    { \
        qInfo() << "Init start" << Class::staticMetaObject.className(); \
        instance_ = new Class(parent, givenParam1, givenParam2); \
        qInfo() << "Init done" << Class::staticMetaObject.className(); \
    } \
    else \
    { \
        qCritical() << "There is already a Class instance"; \
    } \
}

#define SINGLETON_INHERITABLE_IMPL(Class) \
SINGLETON_IMPL_COMMON(Class) \
\
void Class::init(Class *instance) \
{ \
    if(!instance_) \
    { \
        instance_ = instance; \
    } \
    else \
    { \
        qCritical() << "There is already a Class instance"; \
    } \
}

#define SINGLETON_INHERITABLE_INHERITED_IMPL(Class, ParentClass) \
SINGLETON_IMPL_COMMON(Class) \
\
void Class::init(Class *instance) \
{ \
    if(!instance_) \
    { \
        instance_ = instance; \
        ParentClass::init(instance_); \
    } \
    else \
    { \
        qCritical() << "There is already a Class instance"; \
    } \
}

#define SINGLETON_INHERITED_IMPL(Class, ParentClass) \
SINGLETON_IMPL_COMMON(Class) \
\
void Class::init(QObject *parent) \
{ \
    if(!instance_) \
    { \
        qInfo() << "Init start" << Class::staticMetaObject.className(); \
        instance_ = new Class(parent); \
        ParentClass::init(instance_); \
        qInfo() << "Init done" << Class::staticMetaObject.className(); \
    } \
    else \
    { \
        qCritical() << "There is already a Class instance"; \
    } \
}

#define SINGLETON_DESTROY_IMPL(Class) \
if(instance_ == this) \
{ \
    instance_ = nullptr; \
} \
else \
{ \
    qCritical() << "Current instance of Class is not this ?!"; \
}

#define SINGLETON_DESTRUCTOR_IMPL(Class) \
Class::~Class() \
{ \
    SINGLETON_DESTROY_IMPL(Class) \
}

// clang-format on
