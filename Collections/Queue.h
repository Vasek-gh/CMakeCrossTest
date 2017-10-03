#ifndef QUEUE_H
#define QUEUE_H

#include "Chunks.h"

namespace GreedyContainers {
namespace Internal {

//##############################################################################
//
// ChunkedQueue
//
//##############################################################################

template<class T, class Alloc, size_t ChunkSize>
class ChunkedQueue : public ChunkedContainer<T, Alloc, ChunkSize>
{
    using Parent = Internal::ChunkedContainer<T, Alloc, ChunkSize>;
    using TPtr = typename Parent::TPtr;
public:
    ChunkedQueue(Alloc& alloc, size_t capacity)
        : Parent(alloc, capacity)
    {
        _headIndex = 0;
    }

    ~ChunkedQueue()
    {
        this->releaseAllObjects(_headIndex);
    }

    TPtr peek()
    {
        this->emptyCheck();
        return this->_head->getItem(_headIndex);
    }

    void dequeue()
    {
        this->emptyCheck();

        this->releaseItem(this->_head, _headIndex);
        this-> _count--;
        _headIndex++;

        if (_headIndex == ChunkSize || this->_count == 0) {
            _headIndex = 0;

            if (this->_head != this->_tail) {
                auto next = this->_head->next;
                this->_pool.release(this->_head, false);
                this->_head = next;
            }
        }
    }

    void clear()
    {
        this->clearAllItems(_headIndex);
    }
private:
    int _headIndex;
};

} // Internal end

//##############################################################################
//
// ObjQueue
//
//##############################################################################

template<class T, class Alloc, size_t ChunkSize = Internal::DEF_CHUNK_SIZE>
class ObjQueue final : public Internal::ChunkedQueue<T, Alloc, ChunkSize>
{
    using Parent = Internal::ChunkedQueue<T, Alloc, ChunkSize>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    using Parent::Parent;

    template<typename ...Args>
    T* enqueue(Args&&... args)
    {
        return this->appendItem(std::forward<Args>(args)...);
    }
};

//##############################################################################
//
// PtrQueue
//
//##############################################################################

template<class T, class Alloc, size_t ChunkSize = Internal::DEF_CHUNK_SIZE>
class PtrQueue final : public Internal::ChunkedQueue<T*, Alloc, ChunkSize>
{
    using Parent = Internal::ChunkedQueue<T*, Alloc, ChunkSize>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    using Parent::Parent;

    void enqueue(T* value)
    {
        this->appendItem(value);
    }
};

}

#endif // QUEUE_H
