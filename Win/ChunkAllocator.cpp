#include "../ChunkAllocator.h"
#include "../Exception.h"
#include <windows.h>

int _pageSize = -1;

void checkChunkAlign(void* chunk)
{
    if (reinterpret_cast<intptr_t>(chunk) % _pageSize > 0) {
        RAISE(BadAllocException, "Chunk address not aligned");
    }
}

void ChunkAllocator::init()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    _pageSize = sysInfo.dwPageSize;
}

void* ChunkAllocator::alloc(int count)
{
    void* result = VirtualAlloc(
        nullptr,
        _pageSize * count,
        MEM_COMMIT,
        PAGE_READWRITE
    );

    if (result == nullptr) {
        // todo throw OS
    }

    checkChunkAlign(result);
    return result;
}

void ChunkAllocator::free(void* chunk)
{
    checkChunkAlign(chunk);
    if (VirtualFree(chunk, 0, MEM_RELEASE) == 0) {
        // ??? todo throw OS
    }
}

int ChunkAllocator::getChunkSize()
{
    return _pageSize;
}
