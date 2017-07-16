#include "Align.h"

#ifdef OS_WINDOWS

#include <malloc.h>

void* getAlignedMemory(size_t size, size_t alignment)
{
    return _aligned_malloc(size, alignment);
}

void freeAlignedMemory(void* memory)
{
    _aligned_free(memory);
}

#endif

#ifdef OS_LINUX

#include <stdlib.h>

void* getAlignedMemory(size_t size, size_t alignment)
{
    return aligned_alloc(alignment, size);
}

void freeAlignedMemory(void* memory)
{
    free(memory);
}

#endif

bool checkAlign(void* value, uintptr_t align)
{
    return reinterpret_cast<uintptr_t>(value) % align == 0;
}

bool checkAlign(uintptr_t value, uintptr_t align)
{
    return value % align == 0;
}
