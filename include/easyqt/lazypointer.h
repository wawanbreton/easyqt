#pragma once

#include <QUuid>
#include <concepts>

namespace easyqt
{

template<class ObjectType>
concept ObjectWithUuid = requires(const ObjectType& object) {
    { object.getUuid() } -> std::same_as<const QUuid&>;
};

template<class ManagerType, class ObjectType>
concept ObjectWithUuidManager = requires(ManagerType& manager, const QUuid& uuid) {
    { manager.find(uuid) } -> std::same_as<ObjectType*>;
};

/*!
 * LazyPointer is a helper class to define a pointer to an object, which can also be pointed to by UUID. This way, you
 * can use only the UUID of the object and resolve it to a pointer at the very last moment, which can be useful in some
 * scenarios where objects point to each other in a loop. The pointer is retrieved by finding the object
 * via its (singleton) manager.
 */

// ObjectType should use the ObjectWithUuid concept, and ManagerType should use the ObjectWithUuidManager concept.
// This is not enforced programmatically since it requires fully declaring the used classes in the headers,
// which can lead to cycling inclusions. The concepts are still declared here for explicitation.
template<class ObjectType, class ManagerType>
class LazyPointer
{
public:
    LazyPointer() = default;

    void operator=(const QUuid& uuid)
    {
        content_.uuid = uuid;
        content_type_ = ContentType::Uuid;
    }

    void operator=(ObjectType* pointer)
    {
        content_.pointer = pointer;
        content_type_ = ContentType::Pointer;
    }

    bool operator==(const ObjectType* object) const
    {
        if (object == nullptr)
        {
            switch (content_type_)
            {
                case ContentType::None:
                    return true;
                case ContentType::Uuid:
                    return content_.uuid.isNull();
                case ContentType::Pointer:
                    return content_.pointer == nullptr;
            }
        }
        else
        {
            switch (content_type_)
            {
                case ContentType::None:
                    return false;
                case ContentType::Uuid:
                    return object->getUuid() == content_.uuid;
                case ContentType::Pointer:
                    return object == content_.pointer;
            }
        }

        return false;
    }

    bool operator!=(const ObjectType* object) const
    {
        return ! operator==(object);
    }

    operator ObjectType*() const
    {
        if (content_type_ != ContentType::Pointer)
        {
            // Method is const by principle because it does not change the pointed object,
            // but it does change the inner data
            auto unconst_this = const_cast<LazyPointer<ObjectType, ManagerType>*>(this);
            if (content_type_ == ContentType::None)
            {
                unconst_this->content_.pointer = nullptr;
            }
            else // content_type_ == ContentType::Uuid
            {
                unconst_this->content_.pointer = ManagerType::access()->find(content_.uuid);
            }
            unconst_this->content_type_ = ContentType::Pointer;
        }

        return content_.pointer;
    }

    QUuid getUuid() const
    {
        switch (content_type_)
        {
            case ContentType::None:
                return QUuid();
            case ContentType::Uuid:
                return content_.uuid;
            case ContentType::Pointer:
                return content_.pointer ? content_.pointer->getUuid() : QUuid();
        }
    }

private:
    enum class ContentType
    {
        None,
        Uuid,
        Pointer
    };

private:
    ContentType content_type_{ ContentType::None };

    union
    {
        QUuid uuid{};
        ObjectType* pointer;
    } content_;
};

} // namespace easyqt
