#ifndef LINEARALLOCATOR_H
#define LINEARALLOCATOR_H

#include <stddef.h>

class STLinearAllocator
{
public:
    STLinearAllocator(bool cleansable);
    STLinearAllocator(bool cleansable, size_t initSize);
    ~STLinearAllocator();

    void* alloc(size_t size);
    void* alloc(size_t size, size_t align);
private:
    void* data;
};

#endif // LINEARALLOCATOR_H
