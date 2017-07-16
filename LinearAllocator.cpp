#include "LinearAllocator.h"
#include "RegionAllocator.h"
#include "Align.h"
#include "Memory.h"

struct RgnInfo
{
    RgnInfo* prev;
    uintptr_t pos;
    uintptr_t last;
};

RgnInfo* allocRegion(size_t size, size_t headerSize, RgnInfo* prev)
{
    void* rgn = RegionAllocator::alloc(size);

    RgnInfo* info = reinterpret_cast<RgnInfo*>(rgn);
    info->pos = Memory::ptrIntInc(rgn, alignToDefault(headerSize));
    info->last = Memory::ptrIntInc(rgn, size);
    info->prev = prev;

    return info;
}

const int DEFAULT_INIT_SIZE = 65536 * 2; // 64kb
const size_t RGN_INFO_SIZE = alignToDefault(sizeof(RgnInfo));

//##############################################################################
//
// LinearAllocatorPrivate
//
//##############################################################################

class LinearAllocatorPrivate
{
public:
    LinearAllocatorPrivate(bool cleansable, RgnInfo* start)
    {
        _start = start;
        _current = start;
        _cleansable = cleansable;
    }

    void* operator new (size_t, void* ptr)
    {
        return ptr;
    }

    void* alloc(size_t size) {
        return alloc(size, DEFAULT_ALIGN);
    }

    void* alloc(size_t size, size_t align) {

        // выравниваем указатель
        uintptr_t pos = alignValue(_current->pos, align);

        if (pos + size <= _current->last) {
            return allocInCurrent(pos, size);
        }
        else {
            return allocInNew(pos, size);
        }
    }

    void clear()
    {
        if (!_cleansable) {
            return;
        }

        RgnInfo* current = _current;
        while (current) {
            RgnInfo* prev = current->prev;
            RegionAllocator::free(current);
            current = prev;
        }
    }

private:
    LinearAllocatorPrivate(LinearAllocatorPrivate&) = delete;
    LinearAllocatorPrivate(LinearAllocatorPrivate&&) = delete;
    LinearAllocatorPrivate(const LinearAllocatorPrivate&) = delete;
    LinearAllocatorPrivate(const LinearAllocatorPrivate&&) = delete;
    void* operator new (size_t) = delete;

    void* allocInNew(uintptr_t pos, size_t size)
    {
        _current = allocRegion(DEFAULT_INIT_SIZE, RGN_INFO_SIZE, _current);
        return allocInCurrent(_current->pos, size);
    }

    void* allocInCurrent(uintptr_t pos, size_t size)
    {
        _current->pos = pos + size;
        return reinterpret_cast<void*>(pos);
    }

    RgnInfo* _start;
    RgnInfo* _current;
    bool _cleansable;
};

//##############################################################################
//
// LinearAllocator
//
//##############################################################################

const size_t PRIVATE_SIZE = alignToDefault(sizeof(LinearAllocatorPrivate));


void* createPrivateData(bool cleansable, size_t initSize)
{
    auto info = allocRegion(initSize, PRIVATE_SIZE + RGN_INFO_SIZE, nullptr);
    auto privateZone = Memory::ptrInc(info, RGN_INFO_SIZE);
    auto allocator = new (privateZone) LinearAllocatorPrivate(cleansable, info);
    return allocator;
}

STLinearAllocator::STLinearAllocator(bool cleansable)
{
    data = createPrivateData(cleansable, DEFAULT_INIT_SIZE);
}

STLinearAllocator::STLinearAllocator(bool cleansable, size_t initSize)
{
    if (initSize < Memory::getPageSize()) {
        initSize = DEFAULT_INIT_SIZE;
    }

    data = createPrivateData(cleansable, initSize);
}

STLinearAllocator::~STLinearAllocator()
{
    reinterpret_cast<LinearAllocatorPrivate*>(data)->clear();
    reinterpret_cast<LinearAllocatorPrivate*>(data)->~LinearAllocatorPrivate();
}

void* STLinearAllocator::alloc(size_t size)
{
    return reinterpret_cast<LinearAllocatorPrivate*>(data)->alloc(size);
}

void* STLinearAllocator::alloc(size_t size, size_t align)
{
    return reinterpret_cast<LinearAllocatorPrivate*>(data)->alloc(size, align);
}
