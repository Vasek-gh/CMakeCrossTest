#include "../Memory.h"
#include "../Exception.h"
#include "../Align.h"
#include "Utils.h"

#include <atomic>
#include <windows.h>

namespace MemoryInternal {

    // vurtual memory page size
    size_t _pageSize = 1;
    // initialization flag
    std::atomic<bool> _initFlag;
}

using namespace MemoryInternal;

void Memory::init()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    _pageSize = sysInfo.dwPageSize;

    _initFlag.store(true);
}

size_t Memory::getPageSize()
{
    return _pageSize;
}

void* Memory::allocRegion(size_t& size)
{
    size = alignValue(size, _pageSize);

    void* region = VirtualAlloc(
        nullptr,
        size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (region == nullptr) {
        RAISE(BadAllocException,
            "VirtualAlloc failed with reason: " + getLastErrorMessage()
        );
    }

    return region;
}

void Memory::freeRegion(void* region, size_t size)
{
    size = 0;
    if (VirtualFree(region, 0, MEM_RELEASE) == 0) {
        RAISE(BadAllocException,
            "VirtualFree failed with reason: " + getLastErrorMessage()
        );
    }
}
