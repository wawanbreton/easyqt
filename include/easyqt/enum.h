#pragma once

#include <QDebug>
#include <QMetaEnum>
#include <QObject>
#include <optional>

namespace easyqt
{

class Enum
{
public:
    inline static quint8 countSetFlags(quint32 flag)
    {
        quint8 count;
        for (count = 0; flag; count++)
        {
            flag &= flag - 1;
        }
        return count;
    }
};

} // namespace easyqt

#define ENUM_DEBUG() \
public: \
    inline friend QDebug operator<<(QDebug dbg, const Enum& value) \
    { \
        return dbg << staticMetaObject.enumerator(0).valueToKey(value); \
    }

#define ENUM_TOSTRING() \
public: \
    inline static QString toString(Enum value) \
    { \
        return QString(staticMetaObject.enumerator(0).valueToKey(value)); \
    }

#define ENUM_GETVALUES() \
public: \
    inline static QList<Enum> getValues(bool includeNone = false) \
    { \
        QList<Enum> result; \
        QMetaEnum me = staticMetaObject.enumerator(0); \
        for (int i = 0; i < me.keyCount(); i++) \
        { \
            Enum value = static_cast<Enum>(me.value(i)); \
            if (QString(me.valueToKey(value)).toLower() != "none" || includeNone) \
            { \
                result << value; \
            } \
        } \
        return result; \
    }

#define ENUM_GETALLVALUES() \
public: \
    inline static QList<Enum> getAllValues() \
    { \
        QList<Enum> result; \
        QMetaEnum me = staticMetaObject.enumerator(0); \
        for (int i = 0; i < me.keyCount(); i++) \
        { \
            result << static_cast<Enum>(me.value(i)); \
        } \
        return result; \
    }

#define ENUM_FROMSTRING() \
public: \
    inline static std::optional<Enum> fromString(const QString& text) \
    { \
        QMetaEnum me = staticMetaObject.enumerator(0); \
        int value = me.keyToValue(text.toUtf8()); \
        if (value >= 0) \
        { \
            return static_cast<Enum>(value); \
        } \
        else \
        { \
            return std::nullopt; \
        } \
    }

#define ENUM_STREAM() \
    inline QDataStream& operator<<(QDataStream& out, Enum value) \
    { \
        return out << toString(value); \
    } \
\
    inline QDataStream& operator>>(QDataStream& in, Enum& value) \
    { \
        QString stringValue; \
        in >> stringValue; \
        value = fromString(stringValue); \
        return in; \
    }

#define FLAGS_TOLIST() \
public: \
    inline static QList<Enum> toList(Flags flags) \
    { \
        QList<Enum> result; \
        foreach (Enum value, getAllValues()) \
        { \
            if (value != 0 && flags.testFlag(value)) \
            { \
                result << value; \
            } \
        } \
        return result; \
    }

#define FLAGS_FROMLIST() \
public: \
    inline static Flags fromList(const QList<Enum>& values) \
    { \
        Flags result; \
        foreach (Enum value, values) \
        { \
            result |= value; \
        } \
        return result; \
    }

#define FLAGS_GETALLFLAGS() \
public: \
    inline static Flags getAllFlags() \
    { \
        return fromList(getValues()); \
    }
