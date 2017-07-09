#ifndef CHUNKALLOCATOR_H
#define CHUNKALLOCATOR_H

class ChunkAllocator
{
public:
    // static only class
    ChunkAllocator() = delete;

    static void init();
    static void* alloc(int count = 1);
    static void free(void* chunk);
    static int getChunkSize();
};

#endif // CHUNKALLOCATOR_H
