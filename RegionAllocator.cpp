#include "RegionAllocator.h"
#include "Align.h"
#include "Debug.h"
#include "Memory.h"
#include "Exception.h"

#include <stdint.h>

namespace RegionAllocatorInternal {

    // размер magic поля
    const int MAGIC_WORD_COUNT = 8;
    // magic константа
    const char MAGIC_WORD[MAGIC_WORD_COUNT] = "_Magic_";
    // указатель на magic
    const void* MAGIC_WORD_PTR = &MAGIC_WORD[0];

    // заголовок региона
    struct Header
    {
        char magicWord[MAGIC_WORD_COUNT];
        size_t byteCount;
    };

    // выровняный размер заголовка
    const size_t HEADER_SIZE = alignToDefault(sizeof(Header));

    void initMagic(void* value)
    {
        if (MAGIC_WORD_COUNT == 8) {
            using SrcPtr = const uint64_t*;
            using DstPtr = uint64_t*;

            SrcPtr src = static_cast<SrcPtr>(MAGIC_WORD_PTR);
            DstPtr dst = static_cast<DstPtr>(value);
            *dst = *src;
        }
        else {
            memcpy(value, MAGIC_WORD_PTR, MAGIC_WORD_COUNT);
        }
    }

    void* initRegion(void* rgn, size_t size)
    {
        ASSERT(size > HEADER_SIZE, "Region size is too small");
        ASSERT(checkAlign(rgn, Memory::getPageSize()), "Region address not aligned");

        Header* header = reinterpret_cast<Header*>(rgn);
        initMagic(&header->magicWord[0]);
        header->byteCount = size;

        return Memory::ptrInc(header, HEADER_SIZE);
    }

    void checkMagic(void* value)
    {
        if (MAGIC_WORD_COUNT == 8) {
            using SrcPtr = const uint64_t*;
            using DstPtr = uint64_t*;

            SrcPtr src = static_cast<SrcPtr>(MAGIC_WORD_PTR);
            DstPtr dst = static_cast<DstPtr>(value);
            if (*dst != *src) {
                RAISE(ArgumentException, "Invalid region address");
            }
        }
    }

    void checkPageAlign(void* page)
    {
        if (!checkAlign(page, Memory::getPageSize())) {
            RAISE(BadAllocException, "Invalid region address");
        }
    }

    Header* getHeader(void* rgn)
    {
        void* rgnStart = Memory::ptrDec(rgn, HEADER_SIZE);
        checkPageAlign(rgnStart);

        Header* header = reinterpret_cast<Header*>(rgnStart);
        checkMagic(&header->magicWord[0]);
        return header;
    }
}

using namespace RegionAllocatorInternal;

void* RegionAllocator::alloc(size_t& size)
{
    void* region = Memory::allocRegion(size);
    region = initRegion(region, size);
    size -= HEADER_SIZE;
    return region;
}

void RegionAllocator::free(void* region)
{
    Header* header = getHeader(region);
    Memory::freeRegion(header, header->byteCount);
}

size_t RegionAllocator::getSize(void* region)
{
    Header* header = getHeader(region);
    return header->byteCount;
}
