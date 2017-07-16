#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

class Memory
{
public:
    static void init();

    static size_t getPageSize();

    static void* allocRegion(size_t& size);

    static void freeRegion(void* region, size_t size);

    static void* ptrInc(void* value, size_t size) {
        return static_cast<uint8_t*>(value) + size;
    }

    static uintptr_t ptrIntInc(void* value, size_t size) {
        uintptr_t tmpValue = reinterpret_cast<uintptr_t>(value);
        return tmpValue + size;
    }

    static void* ptrDec(void* value, size_t size) {
        return static_cast<char*>(value) - size;
    }

private:
    Memory() = delete;
};

#endif // MEMORY_H
