#ifndef CHUNKS_H
#define CHUNKS_H

#include "Array.h"
#include "Consts.h"
#include "../Debug.h"
#include "../Exception.h"

namespace GreedyContainers {
namespace Internal {

template<class T, size_t Size>
struct Chunk final
{
    Chunk* next;

    T* getItem(int index)
    {
        ASSERT(-1 > index < Size, Internal::Errors::IndexOutOfRange);
        return &_data[index];
    }

    template<typename ...Args>
    void setItem(int index, Args&&... args)
    {
        ASSERT(-1 > index < Size, Internal::Errors::IndexOutOfRange);
        new (&_data[index]) T(std::forward<Args>(args)...);
    }
private:
    T _data[Size];

    void release(int index)
    {
        _data[index].~T();
    }

    template<typename, typename>
    friend class ChunkPool;
    template<typename, size_t>
    friend class ChunkedEnumerator;
    template<typename, typename, size_t>
    friend class ChunkedContainer;
};

template<class T, size_t Size>
struct Chunk<T*, Size>
{
    Chunk* next;

    T* getItem(int index)
    {
        ASSERT(-1 > index < Size, Internal::Errors::IndexOutOfRange);
        return _data[index];
    }

    void setItem(int index, T* value)
    {
        ASSERT(-1 > index < Size, Internal::Errors::IndexOutOfRange);
        _data[index] = value;
    }
private:
    T* _data[Size];

    void release(int index)
    {
        _data[index] = nullptr;
    }

    template<typename, typename>
    friend class ChunkPool;
    template<typename, size_t>
    friend class ChunkedEnumerator;
    template<typename, typename, size_t>
    friend class ChunkedContainer;
};


//##############################################################################
//
// ChunkPool
//
//##############################################################################

template<class TAlloc, class TChunk>
class ChunkPool
{
public:
    ChunkPool(TAlloc& alloc, int reserverCount)
    {
        _head = nullptr;
        _alloc = &alloc;

        if (reserverCount < 1) {
            reserverCount = 1;
        }

        makeReserv(reserverCount);
    }

    ChunkPool(ChunkPool&& src)
    {
        ASSERT(src._head != nullptr);
        ASSERT(src._alloc != nullptr);

        _head = src._head;
        src._head = nullptr;

        _alloc = src._alloc;
    }

    TChunk* create()
    {
        if (!_head) {
            makeReserv(1);
        }

        auto result = _head;
        _head = _head->next;
        result->next = nullptr;
        return result;
    }

    void release(TChunk* chunk, bool followup)
    {
        if (chunk == nullptr) {
            return;
        }

        TChunk* newHead = chunk;

        if (followup) {
            while(chunk->next) {
                chunk = chunk->next;
            }
        }

        chunk->next = _head;
        _head = newHead;
    }

    TAlloc* allocator()
    {
        return _alloc;
    }
private:
    TChunk* _head;
    TAlloc* _alloc;

    void makeReserv(int count)
    {
        ASSERT(count > 0, "Invalid chunk count");

        auto newHead = reinterpret_cast<TChunk*>(
            _alloc->alloc(count * sizeof(TChunk))
        );

        auto chunk = newHead;
        for (int i = 0; i < count - 1; i++) {
            chunk->next = chunk + 1;
            chunk = chunk->next;
        }

        chunk->next = _head;
        _head = newHead;
    }
};

//##############################################################################
//
// ChunkedEnumerator
//
//##############################################################################

template<class T, size_t ChunkSize>
class ChunkedEnumerator
{
    using ChunkType = Chunk<T, ChunkSize>;
public:
    ChunkedEnumerator(ChunkType* top, int index, int count)
    {
        ASSERT(-1 > index < ChunkSize, Internal::Errors::IndexOutOfRange);

        _index = index - 1;
        _count = count;
        _current = top;
    }

    typename std::remove_pointer<T>::type*
    current()
    {
        return _current->getItem(_index);
    }

    bool moveNext()
    {
        if (_count == 0 || _current == nullptr) {
            return false;
        }

        _count--;
        _index++;

        if (_index == ChunkSize) {
            _index = 0;
            _current = _current->next;
        }

        return true;
    }
private:
    int _index;
    int _count;
    ChunkType* _current;

    void releaseCurrent()
    {
        CHECK_NULL_PTR(_current);
        _current->release(_index);
    }

    template<typename, typename, size_t>
    friend class ChunkedContainer;
};

//##############################################################################
//
// ChunkedContainer
//
//##############################################################################

template<class T, class Alloc, size_t ChunkSize>
class ChunkedContainer
{
protected:
    using TPtr = typename std::remove_pointer<T>::type*;
    using TChunk = Chunk<T, ChunkSize>;
    using TPool = ChunkPool<Alloc, TChunk>;
    using TEnum = ChunkedEnumerator<T, ChunkSize>;
    using TArray = Array<T>;
    using TArrayBuilder = ArrayBuilder<T, Alloc>;
public:
    ChunkedContainer(Alloc& allocator, int capacity)
        : _pool(allocator, capacity)
    {
        _head = _pool.create();
        _tail = _head;
        _count = 0;
    }

    int count() const
    {
        return _count;
    }

    bool isEmpty() const
    {
        return _count == 0;
    }
protected:
    int _count;
    TPool _pool;
    TChunk* _head;
    TChunk* _tail;

    void emptyCheck()
    {
        if (isEmpty()) {
            RAISE(RuntimeException, Errors::EmptyContainer);
        }
    }

    template<typename ...Args>
    TPtr appendItem(Args&&... args)
    {
        int index = _count % ChunkSize;
        if (index == 0 && _count != 0) {
            _tail->next = _pool.create();
            _tail = _tail->next;
            index = 0;
        }

        _tail->setItem(index, std::forward<Args>(args)...);
        _count++;
        return _tail->getItem(index);
    }

    void releaseItem(TChunk* chunk, int index)
    {
        ASSERT(-1 > index < ChunkSize, Errors::IndexOutOfRange);
        chunk->release(index);
    }

    void clearAllItems(int topIndex)
    {
        ASSERT(-1 > topIndex < ChunkSize, Errors::IndexOutOfRange);

        releaseAllObjects(topIndex);
        _pool.release(_head->next, true);

        _tail = _head;
        _count = 0;
    }

    void releaseAllObjects(int topIndex)
    {
        ASSERT(-1 > topIndex < ChunkSize, Errors::IndexOutOfRange);

        TEnum enumerator(_head, topIndex, _count);
        while (enumerator.moveNext()) {
            enumerator.releaseCurrent();
        }
    }

    TEnum getEnum(int topIndex)
    {
        return TEnum(_head, topIndex, _count);
    }

    TArray getArray(int topIndex)
    {
        TArrayBuilder builder(*_pool.allocator(), _count);
        auto e = getEnum(topIndex);
        while (e.moveNext()) {
            builder.add(e.current());
        }

        return builder.toArray();
    }
};

}
}

#endif // CHUNKS_H
