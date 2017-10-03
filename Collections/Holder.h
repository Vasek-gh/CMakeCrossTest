#ifndef HOLDER_H
#define HOLDER_H

#include "Chunks.h"

namespace GreedyContainers {
namespace Internal {

//##############################################################################
//
// ChunkedStack
//
//##############################################################################

template<class T, class Alloc, size_t ChunckSize>
class ChunkedHolder : public ChunkedContainer<T, Alloc, ChunckSize>
{
    using Parent = Internal::ChunkedContainer<T, Alloc, ChunckSize>;
    using TPtr = typename Parent::TPtr;
    using TChunk = typename Parent::TChunk;
    using TArray = typename Parent::TArray;
public:
    using Enumerator = typename Parent::TEnum;

    ChunkedHolder(Alloc& alloc, size_t capacity)
        : Parent(alloc, capacity)
    {
    }

    ~ChunkedHolder()
    {
        this->releaseAllObjects(0);
    }

    void clear()
    {
        this->clearAllItems(0);
    }

    Enumerator getEnumerator()
    {
        return this->getEnum(0);
    }

    TArray toArray()
    {
        return this->getArray(0);
    }
};

} // Internal end

//##############################################################################
//
// ObjHolder
//
//##############################################################################

template<class T, class Alloc, size_t ChunckSize = Internal::DEF_CHUNK_SIZE>
class ObjHolder final : public Internal::ChunkedHolder<T, Alloc, ChunckSize>
{
    using Parent = Internal::ChunkedHolder<T, Alloc, ChunckSize>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    using Parent::Parent;

    template<typename ...Args>
    T* add(Args&&... args)
    {
        return this->appendItem(std::forward<Args>(args)...);
    }
};

//##############################################################################
//
// PtrHolder
//
//##############################################################################

template<class T, class Alloc, size_t ChunckSize = Internal::DEF_CHUNK_SIZE>
class PtrHolder final : public Internal::ChunkedHolder<T*, Alloc, ChunckSize>
{
    using Parent = Internal::ChunkedHolder<T*, Alloc, ChunckSize>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    using Parent::Parent;

    void add(T* value)
    {
        this->appendItem(value);
    }
};

}

#endif // HOLDER_H
