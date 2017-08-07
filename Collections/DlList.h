#ifndef GDLBASELIST_H
#define GDLBASELIST_H

#include <stdint.h>

#include "../Debug.h"
#include "../Exception.h"

#include "Base.h"
#include "Consts.h"

namespace GreedyCollections {
namespace Internal {

template<class T, class Allocator>
class BaseDoubleLinkedList
{
public:
    using NodeType = DlNode<T>;
    using NodeAllocator = NodeAllocator<NodeType, Allocator>;
    using ItemHelper = ItemHelper<T, NodeType>;
public:
    using IterType = DlIterForward<T>;
    using ItemType = Item<T, NodeType>;

    ~BaseDoubleLinkedList()
    {
        releaseAll();
    }

    void remove(ItemType& item)
    {
        removeNode(ItemHelper::getNode(item));
        ItemHelper::release(item);
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
        return &_tail;
    }
protected:
    BaseDoubleLinkedList(Allocator& allocator, size_t chunkSize, size_t capacity)
        : _nodeAllocator(allocator, chunkSize, capacity)
    {
        initData();
    }

    BaseDoubleLinkedList(BaseDoubleLinkedList&& src)
        : _nodeAllocator(std::move(src._nodeAllocator))
    {
        _head.prev = nullptr;
        _head.value = nullptr;
        _tail.next = nullptr;
        _tail.value = nullptr;
        _count = src._count;

        auto srcHead = &src._head;
        auto srcTail = &src._tail;
        connectNodes(&_head, srcHead->next);
        connectNodes(srcTail->prev, &_tail);

        src._count = 0;
        connectNodes(srcHead, srcTail);
    }

    template<typename ...Args>
    ItemType doAddFirst(Args&&... args)
    {
        auto node = insertNode(&_head, _head.next, std::forward<Args>(args)...);
        return ItemHelper::make(node);
    }

    template<typename ...Args>
    ItemType doAddLast(Args&&... args)
    {
        auto node = insertNode(_tail.prev, &_tail, std::forward<Args>(args)...);
        return ItemHelper::make(node);
    }

    template<typename ...Args>
    ItemType doInsertBefore(ItemType& next, Args&&... args)
    {
        auto nextNode = ItemHelper::getNode(next);
        checkNode(nextNode);
        auto node = insertNode(nextNode->prev, nextNode, std::forward<Args>(args)...);
        return ItemHelper::make(node);
    }

    template<typename ...Args>
    ItemType doInsertAfter(ItemType& prev, Args&&... args)
    {
        auto prevNode = ItemHelper::getNode(prev);
        checkNode(prevNode);
        auto node = insertNode(prevNode, prevNode->next, std::forward<Args>(args)...);
        return ItemHelper::make(node);
    }

private:
    size_t _count;
    NodeType _head;
    NodeType _tail;
    NodeAllocator _nodeAllocator;

    void initData()
    {
        _count = 0;

        _head.prev = nullptr;
        _head.next = &_tail;
        _head.value = nullptr;

        _tail.prev = &_head;
        _tail.next = nullptr;
        _tail.value = nullptr;
    }

    template<typename ...Args>
    NodeType* insertNode(NodeType* prev, NodeType* next, Args&&... args)
    {
        ASSERT(prev != nullptr);
        ASSERT(next != nullptr);
        checkNode(prev);

        auto node = _nodeAllocator.create(std::forward<Args>(args)...);
        connectNodes(prev, node);
        connectNodes(node, next);
        _count++;

        return node;
    }

    void removeNode(NodeType* node)
    {
        ASSERT(node->prev != nullptr && node->next != nullptr);
        checkNode(node);

        auto prev = node->prev;
        auto next = node->next;
        _nodeAllocator.release(node);
        connectNodes(prev, next);
        _count--;
    }

    void connectNodes(NodeType* first, NodeType* second)
    {
        ASSERT(first != nullptr);
        ASSERT(second != nullptr);

        first->next = second;
        second->prev = first;
    }

    void checkNode(NodeType* node)
    {
        ASSERT(node != nullptr);

        if (node == &_head || node == &_tail) {
            return;
        }

        auto current = _head.next;
        while (current != &_tail) {
            if (current == node) {
                return;
            }

            current = current->next;
        }

        RAISE(ArgumentException, Errors::UnknownNode);
    }

    void releaseAll()
    {
        auto current = _head.next;
        while (current != &_tail) {
            auto next = current->next;
            _nodeAllocator.release(current);
            current = next;
        }
    }
};

} // Internal end

//##############################################################################
//
// DlObjList
//
//##############################################################################

template<class T, class Allocator>
class DlObjList : public Internal::BaseDoubleLinkedList<T, Allocator>
{
    using Parent = Internal::BaseDoubleLinkedList<T, Allocator>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    using Item = typename Parent::ItemType;
    using Iter = typename Parent::IterType;

    DlObjList(Allocator& allocator, size_t chunkSize, size_t capacity)
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
// DlPtrList
//
//##############################################################################

template<class T, class Allocator>
class DlPtrList: public Internal::BaseDoubleLinkedList<T*, Allocator>
{
    using Parent = Internal::BaseDoubleLinkedList<T*, Allocator>;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    using Item = typename Parent::ItemType;
    using Iter = typename Parent::IterType;

    DlPtrList(Allocator& allocator, size_t chunkSize, size_t capacity)
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

#endif // GDLBASELIST_H
