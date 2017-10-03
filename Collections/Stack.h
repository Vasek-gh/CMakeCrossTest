#ifndef STACK_H
#define STACK_H

#include "Chunks.h"

namespace GreedyContainers {
namespace Internal {

//##############################################################################
//
// ChunkedStack
//
//##############################################################################

template<class T, class Alloc, size_t ChunkSize>
class ChunkedStack : public ChunkedContainer<T, Alloc, ChunkSize>
{
    using Parent = Internal::ChunkedContainer<T, Alloc, ChunkSize>;
    using TPtr = typename Parent::TPtr;
    using TChunk = typename Parent::TChunk;
    using TArray = typename Parent::TArray;
public:
    using Enumerator = typename Parent::TEnum;

    ChunkedStack(Alloc& alloc, size_t capacity)
        : Parent(alloc, capacity)
    {
    }

    ~ChunkedStack()
    {
        this->releaseAllObjects(0);
    }

    TPtr peek()
    {
        this->emptyCheck();
        return this->_tail->getItem(getTailIndex());
    }

    void pop()
    {
        this->emptyCheck();

        auto index = getTailIndex();
        this->releaseItem(this->_tail, index);
        this->_count--;

        if (index == 0 && this->_tail != this->_head) {
            auto prev = getTailPrev();
            prev->next = nullptr;
            this->_pool.release(this->_tail, false);
            this->_tail = prev;
        }
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
private:
    int getTailIndex()
    {
        return (this->_count - 1) % ChunkSize;
    }

    TChunk* getTailPrev()
    {
        if (this->_tail == this->_head) {
            return this->_head;
        }

        auto current = this->_head;
        while (current->next != this->_tail) {
            current = current->next;
        }

        return current;
    }
};

} // Internal end

//##############################################################################
//
// ObjStack
//
//##############################################################################

template<class T, class Alloc, size_t ChunkSize = Internal::DEF_CHUNK_SIZE>
class ObjStack final : public Internal::ChunkedStack<T, Alloc, ChunkSize>
{
    using Parent = Internal::ChunkedStack<T, Alloc, ChunkSize>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    using Parent::Parent;

    template<typename ...Args>
    T* push(Args&&... args)
    {
        return this->appendItem(std::forward<Args>(args)...);
    }
};

//##############################################################################
//
// PtrStack
//
//##############################################################################

template<class T, class Alloc, size_t ChunkSize = Internal::DEF_CHUNK_SIZE>
class PtrStack final : public Internal::ChunkedStack<T*, Alloc, ChunkSize>
{
    using Parent = Internal::ChunkedStack<T*, Alloc, ChunkSize>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    using Parent::Parent;

    void push(T* value)
    {
        this->appendItem(value);
    }
};

}

#endif // STACK_H
