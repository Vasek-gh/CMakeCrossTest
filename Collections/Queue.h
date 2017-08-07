#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#include "Base.h"
#include "Consts.h"
#include "../Exception.h"

namespace GreedyCollections {
namespace Internal {

//##############################################################################
//
// BaseQueue
//
//##############################################################################

template<class T, class Allocator>
class BaseQueue
{
    using NodeType = SlNode<T>;
    using ItemHelper = ItemHelper<T, NodeType>;
    using NodeAllocator = NodeAllocator<NodeType, Allocator>;
public:
    using IterType = SlIter<T>;
    using ItemType = Item<T, NodeType>;

    ~BaseQueue()
    {
        auto current = _head.next;
        while (current) {
            auto next = current->next;
            _nodeAllocator.release(current);
            current = next;
        }
    }

    ItemType peek()
    {
        if (_head.next) {
            return ItemHelper::make(_head.next);
        }

        RAISE(RuntimeException, Errors::EmptyContainer);
    }

    void dequeue()
    {
        doDequeue();
    }

    void clear()
    {
        while (_head.next) {
            doDequeue();
        }
    }

    size_t count()
    {
        return _count;
    }

    IterType begin()
    {
        return _head.next;
    }

    IterType end()
    {
        return nullptr;
    }

protected:
    BaseQueue(Allocator& allocator, size_t chunkSize, size_t capacity)
        : _nodeAllocator(allocator, chunkSize, capacity)
    {
        _tail = &_head;
        _head.next = nullptr;
        _head.value = nullptr;
        _count = 0;
    }

    BaseQueue(BaseQueue&& src)
        : _nodeAllocator(std::move(src._nodeAllocator))
    {
        _tail = src._tail;
        _head.next = src._head.next;
        _head.value = nullptr;
        _count = src._count;

        src._tail = &src._head;
        src._head.next = nullptr;
        src._count = 0;
    }

    template<typename ...Args>
    ItemType doEnqueue(Args&&... args)
    {
        auto node = _nodeAllocator.create(std::forward<Args>(args)...);
        node->next = nullptr;
        _tail->next = node;
        _tail = node;
        _count++;

        return ItemHelper::make(node);
    }

    void doDequeue()
    {
        if (_head.next) {
            auto next = _head.next->next;
            _nodeAllocator.release(_head.next);
            _head.next = next;
            _count--;
        }
    }

private:
    size_t _count;
    NodeType _head;
    NodeType* _tail;
    NodeAllocator _nodeAllocator;
};

} // Internal end

//##############################################################################
//
// ObjQueue
//
//##############################################################################

template<class T, class Allocator>
class ObjQueue final : public Internal::BaseQueue<T, Allocator>
{
    using Parent = Internal::BaseQueue<T, Allocator>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );

public:
    using Item = typename Parent::ItemType;
    using Iter = typename Parent::IterType;

    ObjQueue(Allocator& allocator, size_t chunkSize, size_t capacity)
        : Parent(allocator, chunkSize, capacity)
    {}

    template<typename ...Args>
    Item enqueue(Args&&... args)
    {
        return this->doEnqueue(std::forward<Args>(args)...);
    }
};

//##############################################################################
//
// PtrQueue
//
//##############################################################################

template<class T, class Allocator>
class PtrQueue final : public Internal::BaseQueue<T*, Allocator>
{
    using Parent = Internal::BaseQueue<T*, Allocator>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );

public:
    using Item = typename Parent::ItemType;
    using Iter = typename Parent::IterType;

    PtrQueue(Allocator& allocator, size_t chunkSize, size_t capacity)
        : Parent(allocator, chunkSize, capacity)
    {}

    Item enqueue(T* value)
    {
        return this->doEnqueue(value);
    }
};

}

#endif // QUEUE_H
