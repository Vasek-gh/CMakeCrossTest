#include "../ChunkAllocator.h"
#include <atomic>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include "Utils.h"
#include "../Exception.h"

// vurtual memory page size
long _pageSize = -1;
// defines behavior when released pages
bool _nonReturnable = false;
// internal counter for statistics
std::atomic<int> _pages;
// initialization flag
std::atomic<bool> _initFlag;


void checkPageAlign(void* page)
{
    if (reinterpret_cast<intptr_t>(page) % _pageSize > 0) {
        RAISE(BadAllocException, "Page address not aligned");
    }
}

void checkInitialization()
{
    if (!_initFlag.load()) {
        RAISE(RuntimeException, "Allocator not initialized");
    }
}

void checkCountArgument(int count)
{
    if (count < 1) {
        RAISE(ArgumentException, "Negative or zero value of the argument");
    }
}

void PageAllocator::init(bool nonReturnable)
{
    _pageSize = sysconf(_SC_PAGESIZE);
    _nonReturnable = nonReturnable;
    _pages.store(0);
    _initFlag.store(true);
}

int PageAllocator::getPageSize()
{
    checkInitialization();
    return _pageSize;
}

void* PageAllocator::alloc(int count)
{
    checkInitialization();
    checkCountArgument(count);

    void* result = mmap(
        0,
        _pageSize * count,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    if (result == MAP_FAILED) {
        RAISE(BadAllocException,
            "Error while trying to allocate pages: " + getLastErrorMessage()
        );
    }

    checkPageAlign(result);
    _pages += count;
    return result;
}

void PageAllocator::free(void* page, int count)
{
    checkInitialization();
    checkCountArgument(count);

    if (_nonReturnable) {
        return;
    }

    checkPageAlign(page);
    if (munmap(page, _pageSize * count) != 0) {
        RAISE(BadAllocException,
            "Error while trying to free pages: " + getLastErrorMessage()
        );
    }

    _pages -= count;
}
