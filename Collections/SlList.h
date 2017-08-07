#ifndef GSLBASELIST_H
#define GSLBASELIST_H

#include <stdint.h>

#include "../Debug.h"
#include "../Exception.h"

#include "Base.h"
#include "Consts.h"

namespace GreedyCollections {
namespace Internal {

//##############################################################################
//
// BaseQueue
//
//##############################################################################

template<class T, class Allocator>
class BaseSingleLinkedList
{
public:
    using NodeType = SlNode<T>;
    using NodeAllocatorType = NodeAllocator<NodeType, Allocator>;
    using ItemHelperType = ItemHelper<T, NodeType>;
public:
    using IterType = SlIter<T>;
    using ItemType = Item<T, NodeType>;

    ~BaseSingleLinkedList()
    {
        releaseAll();
    }

    void remove(ItemType& item)
    {
        removeNode(ItemHelperType::getNode(item));
        ItemHelperType::release(item);
    }

    void clear()
    {
        releaseAll();
        initData();
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
    BaseSingleLinkedList(Allocator& allocator, size_t chunkSize, size_t capacity)
        : _nodeAllocator(allocator, chunkSize, capacity)
    {
        initData();
    }

    BaseSingleLinkedList(BaseSingleLinkedList&& src)
        : _nodeAllocator(std::move(src._nodeAllocator))
    {
        _last = src._last;
        _head.next = src._head.next;
        _head.value = nullptr;
        _count = src._count;

        src.initData();
    }

    template<typename ...Args>
    ItemType doAddFirst(Args&&... args)
    {
        auto node = insertNode(&_head, std::forward<Args>(args)...);
        return ItemHelperType::make(node);
    }

    template<typename ...Args>
    ItemType doAddLast(Args&&... args)
    {
        auto node = insertNode(_last, std::forward<Args>(args)...);
        return ItemHelperType::make(node);
    }

    template<typename ...Args>
    ItemType doInsertBefore(ItemType& next, Args&&... args)
    {
        auto nextNode = ItemHelperType::getNode(next);
        auto node = insertNode(getPrev(nextNode), std::forward<Args>(args)...);
        return ItemHelperType::make(node);
    }

    template<typename ...Args>
    ItemType doInsertAfter(ItemType& prev, Args&&... args)
    {
        auto prevNode = ItemHelperType::getNode(prev);
        getPrev(prevNode);
        auto node = insertNode(prevNode, std::forward<Args>(args)...);
        return ItemHelperType::make(node);
    }

private:
    size_t _count;
    NodeType _head;
    NodeType* _last;
    NodeAllocatorType _nodeAllocator;

    void initData()
    {
        _last = &_head;
        _head.next = nullptr;
        _head.value = nullptr;
        _count = 0;
    }

    template<typename ...Args>
    NodeType* insertNode(NodeType* prev, Args&&... args)
    {
        auto node = _nodeAllocator.create(std::forward<Args>(args)...);

        node->next = prev->next;
        prev->next = node;
        _count++;
        if (node->next == nullptr) {
            _last = node;
        }

        return node;
    }

    void removeNode(NodeType* node)
    {
        auto prev = getPrev(node);
        auto next = node->next;
        _nodeAllocator.release(node);

        _count--;
        prev->next = next;
        if (prev->next == nullptr) {
            _last = prev;
        }
    }

    NodeType* getPrev(NodeType* node)
    {
        ASSERT(node != nullptr);

        auto current = &_head;
        while (current) {
            if (current->next == node) {
                return current;
            }

            current = current->next;
        }

        RAISE(ArgumentException, Errors::UnknownNode);
    }

    void releaseAll()
    {
        auto current = _head.next;
        while (current) {
            auto next = current->next;
            _nodeAllocator.release(current);
            current = next;
        }
    }
};

} // Internal end

//##############################################################################
//
// SlObjList
//
//##############################################################################

template<class T, class Allocator>
class SlObjList : public Internal::BaseSingleLinkedList<T, Allocator>
{
    using Parent = Internal::BaseSingleLinkedList<T, Allocator>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    using Item = typename Parent::ItemType;
    using Iter = typename Parent::IterType;

    SlObjList(Allocator& allocator, size_t chunkSize, size_t capacity)
        : Parent(allocator, chunkSize, capacity)
    {}

    template<typename ...Args>
    Item addFirst(Args&&... args)
    {
        return this->doAddFirst(std::forward<Args>(args)...);
    }

    template<typename ...Args>
    Item addLast(Args&&... args)
    {
        return this->doAddLast(std::forward<Args>(args)...);
    }

    template<typename ...Args>
    Item insertBefore(Item& next, Args&&... args)
    {
        return this->doInsertBefore(next, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    Item insertAfter(Item& prev, Args&&... args)
    {
        return this->doInsertAfter(prev, std::forward<Args>(args)...);
    }
};

//##############################################################################
//
// SlPtrList
//
//##############################################################################

template<class T, class Allocator>
class SlPtrList : public Internal::BaseSingleLinkedList<T*, Allocator>
{
    using Parent = Internal::BaseSingleLinkedList<T*, Allocator>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );

public:
    using Item = typename Parent::ItemType;
    using Iter = typename Parent::IterType;

    SlPtrList(Allocator& allocator, size_t chunkSize, size_t capacity)
        : Parent(allocator, chunkSize, capacity)
    {}

    Item addFirst(T* value)
    {
        return this->doAddFirst(value);
    }

    Item addLast(T* value)
    {
        return this->doAddLast(value);
    }

    Item insertBefore(Item& next, T* value)
    {
        return this->doInsertBefore(next, value);
    }

    Item insertAfter(Item& prev, T* value)
    {
        return this->doInsertAfter(prev, value);
    }
};

}

#endif // GSLBASELIST_H
