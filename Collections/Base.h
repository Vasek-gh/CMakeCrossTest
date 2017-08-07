#ifndef BASE_H
#define BASE_H

#include <utility>
#include <stdint.h>
#include <algorithm>
#include <type_traits>

#include "Consts.h"
#include "../Align.h"
#include "../Debug.h"
#include "../Exception.h"

namespace GreedyCollections {
namespace Internal {

//##############################################################################
//
// SlNode
//
//##############################################################################

template<class T>
struct SlNode
{
    T* value;
    SlNode* next;

    template<typename ...Args>
    void init(Args&&... args)
    {
        auto value = new (getDataPtr()) T(std::forward<Args>(args)...);
        this->value = value;
    }

    void release()
    {
        this->value->~T();
        this->value = nullptr;
    }

    static constexpr size_t getDataSize() {
        return alignToDefault(sizeof(T)) + alignToDefault(sizeof(SlNode));
    }
private:
    void* getDataPtr()
    {
        return Memory::ptrInc(this, alignToDefault(sizeof(SlNode)));
    }
};

template<class T>
struct SlNode<T*>
{
    T* value;
    SlNode* next;

    void init(T* value)
    {
        this->value = value;
    }

    void release()
    {
        this->value = nullptr;
    }

    static constexpr size_t getDataSize() {
        return alignToDefault(sizeof(SlNode));
    }
};

//##############################################################################
//
// SlIter
//
//##############################################################################

template<class T>
class SlIter
{
    using NodeType = SlNode<T>;
public:
    SlIter(NodeType* node)
    {
        _node = node;
    }

    bool operator != (SlIter const& other) const
    {
        return _node != other._node;
    }

    SlIter& operator++() {
        _node = _node->next;
        return *this;
    }

    typename std::remove_pointer<T>::type*
    operator*() const
    {
        return _node->value;
    }

private:
    NodeType* _node;
};

//##############################################################################
//
// DlNode
//
//##############################################################################

template<class T>
struct DlNode
{
    T* value;
    DlNode* next;
    DlNode* prev;

    template<typename ...Args>
    void init(Args&&... args)
    {
        auto value = new (getDataPtr()) T(std::forward<Args>(args)...);
        this->value = value;
    }

    void release()
    {
        this->value->~T();
        this->value = nullptr;
    }

    static constexpr size_t getDataSize() {
        return alignToDefault(sizeof(T)) + alignToDefault(sizeof(DlNode));
    }
private:
    void* getDataPtr()
    {
        return Memory::ptrInc(this, alignToDefault(sizeof(DlNode)));
    }
};

template<class T>
struct DlNode<T*>
{
    T* value;
    DlNode* next;
    DlNode* prev;

    void init(T* value)
    {
        this->value = value;
    }

    void release()
    {
        this->value = nullptr;
    }

    static constexpr size_t getDataSize() {
        return alignToDefault(sizeof(DlNode));
    }
};

//##############################################################################
//
// DlIter...
//
//##############################################################################

template<class T>
class DlIterForward
{
    using NodeType = DlNode<T>;
public:
    DlIterForward(NodeType* node)
    {
        _node = node;
    }

    bool operator != (DlIterForward const& other) const
    {
        return _node != other._node;
    }

    DlIterForward& operator++() {
        _node = _node->next;
        return *this;
    }

    typename std::remove_pointer<T>::type*
    operator*() const
    {
        return _node->value;
    }

private:
    NodeType* _node;
};

//##############################################################################
//
// NodeAllocator
//
//##############################################################################

template<class T, class NodeType>
class ItemHelper;

template<class T, class NodeType>
class Item
{
public:
    Item(Item&& src)
    {
        CHECK_NULL_ARG(src._node);
        _node = src._node;
        src._node = nullptr;
    }

    typename std::remove_pointer<T>::type*
    operator ->() const
    {
        CHECK_NULL_PTR(_node);
        CHECK_NULL_PTR(_node->value);
        return _node->value;
    }

private:
    Item() = delete;
    Item(Item& src) = delete;
    Item(const Item& src) = delete;
protected:
    NodeType* _node;

    Item(NodeType* node)
    {
        ASSERT(node != nullptr);
        ASSERT(node->value != nullptr);

        _node = node;
    }

    void release()
    {
        _node = nullptr;
    }

    friend class ItemHelper<T, NodeType>;
};

template<class T, class NodeType>
class ItemHelper
{
    using ItemType = Item<T, NodeType>;
public:
    static ItemType make(NodeType* node)
    {
        return node;
    }

    static void release(ItemType& item)
    {
        item._node = nullptr;
    }

    static NodeType* getNode(ItemType& item)
    {
        return item._node;
    }
};

//##############################################################################
//
// NodeAllocator
//
//##############################################################################

template<class NodeType, class Allocator>
class NodeAllocator
{
public:
    NodeAllocator(Allocator& allocator, size_t chunkSize, size_t capacity)
    {
        _top = nullptr;
        _allocator = &allocator;

        _chunkSize = chunkSize;
        _chunkSize = std::max(_chunkSize, MIN_CHUNK_SIZE);
        _chunkSize = std::min(_chunkSize, MAX_CHUNK_SIZE);

        allocChunk(capacity);
    }

    NodeAllocator(NodeAllocator&& src)
    {
        ASSERT(src._top != nullptr);
        ASSERT(src._allocator != nullptr);
        ASSERT(src._chunkSize >= MIN_CHUNK_SIZE);
        ASSERT(src._chunkSize <= MAX_CHUNK_SIZE);

        _top = src._top;
        _allocator = src._allocator;
        _chunkSize = src._chunkSize;

        src._top = nullptr;
    }

    template<typename ...Args>
    NodeType* create(Args&&... args)
    {
        allocChunk(_chunkSize);

        // In order not to damage internal states when exception raised, it
        // is necessary to call the client's constructor first.
        _top->init(std::forward<Args>(args)...);

        auto result = _top;
        _top = _top->next;
        return result;
    }

    void release(NodeType* node)
    {
        CHECK_NULL_ARG(node);

        node->release();
        node->next = _top;
        _top = node;
    }
private:
    size_t _chunkSize;
    NodeType* _top;
    Allocator* _allocator;

    void allocChunk(size_t size)
    {
        if (_top || size == 0) {
            return;
        }

        _top = reinterpret_cast<NodeType*>(
            _allocator->alloc(size * NodeType::getDataSize())
        );

        auto entry = _top;
        for (int i = 0; i < size - 1; i++) {
            entry->next = reinterpret_cast<NodeType*>(
                Memory::ptrInc(entry, NodeType::getDataSize())
            );

            entry = entry->next;
        }

        entry->next = nullptr;

        CHECK_NULL_PTR(_top);
    }
};

}
}

#endif // BASE_H
