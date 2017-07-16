#ifndef ALIGN_H
#define ALIGN_H

#include <stddef.h>
#include <stdint.h>

const static int PTR_SIZE = sizeof(void*);
const static int DEFAULT_ALIGN = PTR_SIZE;

void* getAlignedMemory(size_t size, size_t alignment);
void freeAlignedMemory(void* memory);

bool checkAlign(void* value, uintptr_t align);
bool checkAlign(uintptr_t value, uintptr_t align);

constexpr uintptr_t alignToDefault(uintptr_t base)
{
    return (base % PTR_SIZE != 0) ? base + PTR_SIZE - base % PTR_SIZE : base;
}

constexpr uintptr_t alignValue(uintptr_t base, uintptr_t allign)
{
    return (base % allign != 0) ? base + allign - base % allign : base;
}

constexpr void* alignValue(void* base, uintptr_t align)
{
    //uintptr_t value = reinterpret_cast<uintptr_t>(base);
    //uintptr_t result = alignValue(reinterpret_cast<uintptr_t>(base), align);
    return reinterpret_cast<void*>(alignValue(reinterpret_cast<uintptr_t>(base), align));
}


#endif // ALIGN_H
