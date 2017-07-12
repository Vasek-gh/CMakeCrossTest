#ifndef CHUNKALLOCATOR_H
#define CHUNKALLOCATOR_H

// Static class API wrapper for allocate virtual memory pages. Before using
// client must invoke "init" method. For maximum performance, the client can
// pass "nonReturnable" argument as true, in which case the allocator will not
// free up memory.
class PageAllocator
{
public:
    // Init allocator.
    static void init(bool nonReturnable);

    // Return page size.
    static int getPageSize();

    // Allocate the specified number of pages.
    static void* alloc(int count = 1);

    // Release the specified number of pages.
    static void free(void* page, int count = 1);
private:
    // static only class
    PageAllocator() = delete;
};

#endif // CHUNKALLOCATOR_H
