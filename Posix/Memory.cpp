#include "../Memory.h"
#include "../Exception.h"
#include "../Align.h"
#include "Utils.h"

#include <atomic>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

namespace MemoryInternal {

    // vurtual memory page size
    size_t _pageSize = 1;
    // initialization flag
    std::atomic<bool> _initFlag;
}

using namespace MemoryInternal;

void Memory::init()
{
    _pageSize = sysconf(_SC_PAGESIZE);
    _initFlag.store(true);
}

size_t Memory::getPageSize()
{
    return _pageSize;
}

void* Memory::allocRegion(size_t& size)
{
    size = alignValue(size, _pageSize);

    void* region = mmap(
        0,
        size,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    if (region == MAP_FAILED) {
        RAISE(BadAllocException,
            "mmap failed with reason: " + getLastErrorMessage()
        );
    }

    return region;
}

void Memory::freeRegion(void* region, size_t size)
{
    if (munmap(region, size) != 0) {
        RAISE(BadAllocException,
            "munmap failed with reason: " + getLastErrorMessage()
        );
    }
}
