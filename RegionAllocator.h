#ifndef REGIONALLOCATOR_H
#define REGIONALLOCATOR_H

#include <stddef.h>

class RegionAllocator
{
public:
    // Выделяет регион достаточный для хранения указаного кол-ва байт.
    // При выделении размер округляется до гранулярности стриниц в системе.
    static void* alloc(size_t& size);

    // Освобождяет указанный регион. Параметр region должен быть равен
    // реузультату alloc.
    static void free(void* region);

    //
    static size_t getSize(void* region);
private:
    // static only class
    RegionAllocator() = delete;
};

#endif // REGIONALLOCATOR_H
