#pragma once

#include <QtGlobal>

class CommandHeader
{
    public:
        CommandHeader(const quint32 &id) :
            _id(id)
        { }

        // Required to mark the class as polymorphic
        virtual ~CommandHeader() = default;

        const quint32 &getId() const { return _id; }

    private:
        const quint32 _id;
};
