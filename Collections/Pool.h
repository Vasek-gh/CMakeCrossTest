#ifndef POOL_H
#define POOL_H

#include "Nodes.h"
#include "Holder.h"

namespace GreedyContainers {

template<class T, class Alloc, size_t ChunkSize>
class Pool
{
    using TNode = Internal::SlNode<T*>;
    using TNodePool = Internal::NodePool<TNode, Alloc>;
    using THolder = ObjHolder<T, Alloc, ChunkSize>;
public:
    class Item
    {
    public:
        Item(Pool* owner, TNode* node)
        {
            _node = node;
            _owner = owner;
        }

        ~Item() {
            _owner->pop(_node);
            _node = nullptr;
            _owner = nullptr;
        }

        T* operator ->() const
        {
            return _node->value;
        }
    private:
        Pool* _owner;
        TNode* _node;
    };
public:
    Pool(Alloc& alloc, size_t capacity)
        : _holder(alloc, 2),
          _nodePool(alloc, ChunkSize, capacity)
    {
        _tail = nullptr;
    }

    template<typename ...Args>
    Item create(Args&&... args)
    {
        TNode* node = nullptr;

        if (_tail != nullptr) {
            node = _tail;
            _tail = _tail->next;
        }
        else {
            auto item = _holder.add(std::forward<Args>(args)...);
            node = _nodePool.create(item);
        }

        node->next = nullptr;
        return Item(this, node);
    }
private:
    TNode* _tail;
    THolder _holder;
    TNodePool _nodePool;

    void pop(TNode* node)
    {
        node->next = _tail;
        _tail = node;
    }

    friend class Item;
};

}

#endif // POOL_H
