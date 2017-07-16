#ifndef OBJECTSTORAGE_H
#define OBJECTSTORAGE_H

#include <new>
#include <stdint.h>
#include "Debug.h"
#include "Exception.h"
#include "Align.h"



//##########################################
//
// ObjectStorage<T>
//  Реализует хранилище объектов без копирования при изменении размера.
//  Основеное назначение объекта заменить вектор в тех местах где нужно
//  просто хранить объекты.
//
//##########################################

template <class T>
class ObjectStorage
{

public:
    ObjectStorage(uint32_t capacity = DEFAULT_CAPACITY)
    {
        _currentChunk = nullptr;
        if (capacity < MIN_CAPACITY) {
            capacity = MIN_CAPACITY;
        }
        allocChunk(capacity);
    }

    ~ObjectStorage()
    {
        auto chunk = _currentChunk;
        while (chunk) {
            auto prevChunk = chunk->prev();
            chunk->release();
            freeAlignedMemory(chunk->entry());
            chunk = prevChunk;
        }
    }

    template<typename ...Args>
    T* create(Args&&... args)
    {
        void* newElement = _currentChunk->getElement();
        if (!newElement) {
            allocChunk(_currentChunk->size() * 2);
            newElement = _currentChunk->getElement();
            if (!newElement) {
                RAISE(BadAllocException, "cannot get element after alloc");
                throw std::bad_alloc();
            }
        }

        return new(newElement) T(std::forward<Args>(args)...);
    }
private:
    struct Chunk
    {
    private:
        Chunk*   _prev;
        void*    _entry;
        uint32_t _size;
        uint32_t _current;
    public:
        void init(Chunk* prev, void* entry, uint32_t size)
        {
            _prev    = prev;
            _entry   = entry;
            _size    = size;
            _current = 0;
        }

        void release()
        {
            for(uint32_t pos = 0; pos < _current; ++pos) {
                T* item = reinterpret_cast<T*>(calcOffset(_entry, pos));
                item->~T();
            }
        }

        void* getElement() {
            if (_current < _size) {
                auto result = calcOffset(_entry, _current);
                ASSERT(reinterpret_cast<uintptr_t>(result) % EL_ALIGN == 0, "bad element pointer");
                _current++;
                return result;
            }

            return nullptr;
        }

        void* entry() { return _entry;}
        Chunk* prev() { return _prev; }
        uint32_t size() { return _size; }
    };

    // размер данных типа
    const static int TP_SIZE = sizeof(T);
    // размер данных чанка
    const static int CH_SIZE = sizeof(Chunk);
    // выравнивание для элемента
    const static int EL_ALIGN = alignof(T);
    // размер занимаемый одним элементом
    const static int EL_SIZE = alignValue(TP_SIZE, EL_ALIGN);
    // минимальное кол-во элементов для выделения чанка
    const static int MIN_CAPACITY = 10;
    // кол-во элементов для выделения чанка по умолчанию
    const static int DEFAULT_CAPACITY = 100;

    void allocChunk(uint32_t size)
    {
        void* chunkEntry = getAlignedMemory(EL_SIZE * size + CH_SIZE, EL_ALIGN);
        if (!chunkEntry) {
            RAISE(BadAllocException, "aligned_alloc fail");
        }

        Chunk* newChunk = (Chunk*)calcOffset(chunkEntry, size);

        ASSERT(reinterpret_cast<uintptr_t>(chunkEntry) % EL_ALIGN == 0, "bad chunk entry pointer");
        ASSERT(reinterpret_cast<uintptr_t>(newChunk) % EL_ALIGN == 0, "bad chunk pointer");

        newChunk->init(_currentChunk, chunkEntry, size);
        _currentChunk = newChunk;
    }

    static void* calcOffset(void* entry, uint32_t count)
    {
        uintptr_t entryInt = reinterpret_cast<uintptr_t>(entry);
        return reinterpret_cast<void*>(entryInt + EL_SIZE * count);
    }

    Chunk* _currentChunk;
};


#endif // OBJECTSTORAGE_H
