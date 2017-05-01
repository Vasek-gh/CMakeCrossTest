#ifndef ALIGN_H
#define ALIGN_H

#include <stddef.h>
#include <stdint.h>

void* getAlignedMemory(size_t size, size_t alignment);
void* freeAlignedMemory(void* memory);

constexpr uintptr_t allign_value(uintptr_t base, uintptr_t allign)
{
    return (base % allign != 0) ? base + allign - base % allign : base;
}

#endif // ALIGN_H
