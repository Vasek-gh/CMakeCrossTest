#ifndef STACK_H
#define STACK_H

#include <stdint.h>

#include "Base.h"
#include "Consts.h"
#include "../Exception.h"

namespace GreedyCollections {
namespace Internal {

//##############################################################################
//
// BaseStack
//
//##############################################################################

template<class T, class Allocator>
class BaseStack
{
    using NodeType = SlNode<T>;
    using NodeAllocatorType = NodeAllocator<NodeType, Allocator>;
    using ItemHelperType = ItemHelper<T, NodeType>;
public:
    using IterType = SlIter<T>;
    using ItemType = Item<T, NodeType>;

    ~BaseStack()
    {
        auto current = _top;
        while (current) {
            auto next = current->next;
            _nodeAllocator.release(current);
            current = next;
        }
    }

    ItemType peek()
    {
        if (_top == nullptr) {
            return ItemHelperType::make(_top);
        }

        RAISE(RuntimeException, Errors::EmptyContainer);
    }

    void pop()
    {
        doPop();
    }

    void clear()
    {
        while (_top) {
            doPop();
        }
    }

    size_t count()
    {
        return _count;
    }

    IterType begin()
    {
        return _top;
    }

    IterType end()
    {
        return nullptr;
    }

protected:
    BaseStack(Allocator& allocator, size_t chunkSize, size_t capacity)
        : _nodeAllocator(allocator, chunkSize, capacity)
    {
        _top = nullptr;
        _count = 0;
    }

    BaseStack(BaseStack&& src)
        : _nodeAllocator(std::move(src._nodeAllocator))
    {
        _top = src._top;
        _count = src._count;

        src._top = nullptr;
        src._count = 0;
    }

    template<typename ...Args>
    ItemType doPush(Args&&... args)
    {
        auto node = _nodeAllocator.create(std::forward<Args>(args)...);

        node->next = _top;
        _top = node;
        _count++;

        return ItemHelperType::make(node);
    }

    void doPop()
    {
        if (_top) {
            auto next = _top->next;
            _nodeAllocator.release(_top);
            _top = next;
            _count--;
        }
    }

private:
    size_t _count;
    NodeType* _top;
    NodeAllocatorType _nodeAllocator;
};

} // Internal end

//##############################################################################
//
// ObjStack
//
//##############################################################################

template<class T, class Allocator>
class ObjStack final : public Internal::BaseStack<T, Allocator>
{
    using Parent = Internal::BaseStack<T, Allocator>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );

public:
    using Item = typename Parent::ItemType;
    using Iter = typename Parent::IterType;

    ObjStack(Allocator& allocator, size_t chunkSize, size_t capacity)
        : Parent(allocator, chunkSize, capacity)
    {}

    template<typename ...Args>
    Item push(Args&&... args)
    {
        return this->doPush(std::forward<Args>(args)...);
    }
};

//##############################################################################
//
// PtrStack
//
//##############################################################################

template<class T, class Allocator>
class PtrStack final : public Internal::BaseStack<T*, Allocator>
{
    using Parent = Internal::BaseStack<T*, Allocator>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );

public:
    using Item = typename Parent::ItemType;
    using Iter = typename Parent::IterType;

    PtrStack(Allocator& allocator, size_t chunkSize, size_t capacity)
        : Parent(allocator, chunkSize, capacity)
    {}

    Item push(T* value)
    {
        return this->doPush(value);
    }
};

}

#endif // STACK_H
