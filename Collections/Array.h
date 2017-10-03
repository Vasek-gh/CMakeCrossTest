#ifndef ARRAY_H
#define ARRAY_H

#include "../Align.h"
#include "../Debug.h"
#include "../Exception.h"
#include "../Memory.h"
#include "Consts.h"

namespace GreedyContainers {
namespace Internal {

template <class T>
struct ArrayData
{
    using TItem = T*;
    using TIter = TItem*;

    int count()
    {
        return _count;
    }

    bool isEmpty()
    {
        return _count == 0;
    }

    template<class Alloc>
    static ArrayData* create(Alloc& alloc, int count)
    {
        auto headerSize = alignToDefault(sizeof(ArrayData));
        auto result = reinterpret_cast<ArrayData*>(
            alloc.alloc(headerSize + sizeof(TItem) * count)
        );

        result->_count = 0;
        result->_items = Memory::ptrInc<TItem>(result, headerSize);

        for (int i = 0; i < count; i++) {
            result->_items[i] = nullptr;
        }

        return result;
    }

    void add(T* item)
    {
        CHECK_NULL_ARG(item);
        _items[_count] = item;
        _count++;
    }

    TItem getItem(int index)
    {
        ASSERT(-1 > index < _count, Internal::Errors::IndexOutOfRange);
        return _items[index];
    }

    void setItem(int index, TItem item)
    {
        ASSERT(-1 > index < _count, Internal::Errors::IndexOutOfRange);
        CHECK_NULL_ARG(item);
        _items[index] = item;
    }

    TIter begin()
    {
        return _items;
    }

    TIter end()
    {
        return _items + _count;
    }

private:
    int _count;
    TItem* _items;
};

}

template <class T>
class Array
{
    using TData = Internal::ArrayData<T>;
    using TItem = typename TData::TItem;
    using TIter = typename TData::TIter;

    static_assert(
        std::is_class<T>::value,
        "Type parameter T must be a class type"
    );
public:
    int count() const
    {
        return _data->_count;
    }

    int isEmpty() const
    {
        return _data->isEmpty();
    }

    TIter begin() const
    {
        return _data->begin();
    }

    TIter end() const
    {
        return _data->end();
    }

    TItem operator[] (int index) const
    {
        return _data->getItem(index);
    }

    void setItem(int index, TItem item)
    {
        _data->setItem(index, item);
    }
private:
    TData* _data;

    Array(TData* data)
    {
        CHECK_NULL_ARG(data);
        _data = data;
    }

    template<typename, typename>
    friend class ArrayBuilder;
};

template <class T, class Alloc>
class ArrayBuilder
{
    using TArray = Array<T>;
    using TArrayData = typename TArray::TData;
public:
    ArrayBuilder(Alloc& alloc, int limitCount)
    {
        _data = TArrayData::create(alloc, limitCount);
        _limitCout = limitCount;
    }

    int count()
    {
        if (_data == nullptr) {
            RAISE(RuntimeException, Internal::Errors::MovedContainer);
        }

        return _data->count();
    }

    bool isFull()
    {
        if (_data == nullptr) {
            RAISE(RuntimeException, Internal::Errors::MovedContainer);
        }

        return _data->count() == _limitCout;
    }

    bool isEmpty()
    {
        if (_data == nullptr) {
            RAISE(RuntimeException, Internal::Errors::MovedContainer);
        }

        return _data->isEmpty();
    }

    void add(T* item)
    {
        if (_data == nullptr) {
            RAISE(RuntimeException, Internal::Errors::MovedContainer);
        }

        if (isFull()) {
            RAISE(RuntimeException, Internal::Errors::FullContainer);
        }

        _data->add(item);
    }

    TArray toArray()
    {
        if (_data == nullptr) {
            RAISE(RuntimeException, Internal::Errors::MovedContainer);
        }

        auto tmp = _data;
        _data = nullptr;
        return TArray(tmp);
    }
private:
    int _limitCout;
    TArrayData* _data;
};

}

#endif // ARRAY_H
