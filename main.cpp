#include <iostream>
#include <signal.h>
#include <stdexcept>
#include <chrono>
#include <vector>
#include "Debug.h"
#include "Memory.h"
#include "Exception.h"

#include "Collections/SlList.h"
#include "Collections/DlList.h"
#include "Collections/Queue.h"
#include "Collections/Stack.h"

#include "TestStorage.h"
#include "Test/TestSTLinearAllocator.h"

using namespace std;

static int objcount = 0;
static int alloccount = 0;
static int disposecount = 0;

void* operator new(size_t size)
{
    objcount++;
    alloccount++;
    return (void*)malloc(size);
}

void operator delete(void* p)
{
    disposecount++;
    objcount--;
    free(p);
}

void testEnv()
{
    cout << "start testEnv" << endl;
    try
    {
#ifdef DEBUG
        cout << "DEBUG" << endl;
#endif

#ifdef RELEASE
        cout << "RELEASE" << endl;
#endif

#ifdef AM32
        cout << "32" << endl;
#endif

#ifdef AM64
        cout << "64" << endl;
#endif

#ifdef OS_WINDOWS
        cout << "Windows" << endl;
#endif

#ifdef OS_LINUX
        cout << "Linux" << endl;
#endif

#ifdef MINGW
        cout << "MinGw" << endl;
#endif
    }
    catch (...) {
        std::cerr << "testEnv exception" << std::endl;
    }
}

void testAssert()
{
    cout << "start testAssert" << endl;
    try
    {
        ASSERT(1 == 0);
    }
    catch (const Exception& e) {
        std::cerr << e.what() << std::endl;
    }

    try
    {
        ASSERT(1 == 0, "gggg");
    }
    catch (const Exception& e) {
        std::cerr << e.what() << std::endl;
    }

    try
    {
        int* i = nullptr;
        CHECK_NULL_ARG(i);
    }
    catch (const Exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void testStorage()
{
    cout << "start testStorage" << endl;
    try
    {
        TestStorage test;
        test.run();
    }
    catch (const Exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void testSTLinearAllocator()
{
    cout << "start testSTLinearAllocator" << endl;

    try
    {
        TestSTLinearAllocator test;
        test.run();
    }
    catch (const Exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void signalHandler(int sig)
{
    throw runtime_error("signalHandler");
}

void test()
{
    int* i = nullptr;
    *i = 1;
}

class Test
{
public:
    Test(int a) {
        this->a = a;
    }

    ~Test() {
        a = 7;
    }

    void test() {

    }

    int a;
};

template<class Impl>
class Allocator
{
public:
    void* alloc(size_t size) {
        return self()->doAlloc(size, DEFAULT_ALIGN);
    }

    void* alloc(size_t size, size_t align) {
        return self()->doAlloc(size, align);
    }
private:
    Impl* self()
    {
        return static_cast<Impl*>(this);
    }
};

class AllocatorImpl1: public Allocator<AllocatorImpl1>
{
    friend class Allocator<AllocatorImpl1>;

    void* doAlloc(size_t size, size_t align) {
        //return this->doAlloc(size, align);
    }
};

class SimpleAllocator
{
public:
    void* alloc(size_t size) {
        return malloc(size);
    }

    void* alloc(size_t size, size_t align) {
        return malloc(size);
    }
};

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");

    //wstring ggg = L"ЫЫЫЫ";

    //wcout << ggg << endl;

    signal(SIGSEGV, signalHandler);

    cout << "start" << endl;

    Memory::init();

    testEnv();
    testAssert();
    testStorage();
    //testSTLinearAllocator();

    try
    {
        SimpleAllocator allocator;

        {
            using SlObjList = GreedyCollections::SlObjList<Test, SimpleAllocator>;
            SlObjList l(allocator, 0, 3);
            SlObjList::Item i1 = l.addFirst(1);
            SlObjList::Item i2 = l.addLast(2);
            SlObjList::Item i3 = l.insertAfter(i1, 3);
            SlObjList::Item i4 = l.insertBefore(i1, 4);

            int h = i1->a;
            l.remove(i1);

            for (auto value : l) {
                cout << value->a << endl;
            }

            SlObjList l2 = move(l);

            for (auto value : l) {
                cout << value->a << endl;
            }

            for (auto value : l2) {
                cout << value->a << endl;
            }
        }

        cout << endl;

        {
            using SlPtrList = GreedyCollections::SlPtrList<Test, SimpleAllocator>;
            SlPtrList l(allocator, 0, 3);

            Test t1(1);
            Test t2(2);
            Test t3(3);
            Test t4(4);

            SlPtrList::Item i1 = l.addFirst(&t1);
            SlPtrList::Item i2 = l.addLast(&t2);
            SlPtrList::Item i3 = l.insertAfter(i1, &t3);
            SlPtrList::Item i4 = l.insertBefore(i1, &t4);

            int h = i1->a;
            l.remove(i1);

            for (auto value : l) {
                cout << value->a << endl;
            }

            SlPtrList l2 = move(l);

            for (auto value : l) {
                cout << value->a << endl;
            }

            for (auto value : l2) {
                cout << value->a << endl;
            }
        }

        cout << endl;

        {
            using DlObjList = GreedyCollections::DlObjList<Test, SimpleAllocator>;
            DlObjList l(allocator, 0, 3);
            DlObjList::Item i1 = l.addFirst(1);
            DlObjList::Item i2 = l.addLast(2);
            DlObjList::Item i3 = l.insertAfter(i1, 3);
            DlObjList::Item i4 = l.insertBefore(i1, 4);

            int h = i1->a;
            l.remove(i1);

            for (auto value : l) {
                cout << value->a << endl;
            }

            DlObjList l2 = move(l);

            for (auto value : l) {
                cout << value->a << endl;
            }

            for (auto value : l2) {
                cout << value->a << endl;
            }
        }

        cout << endl;

        {
            using DlPtrList = GreedyCollections::DlPtrList<Test, SimpleAllocator>;
            DlPtrList l(allocator, 0, 3);

            Test t1(1);
            Test t2(2);
            Test t3(3);
            Test t4(4);

            DlPtrList::Item i1 = l.addFirst(&t1);
            DlPtrList::Item i2 = l.addLast(&t2);
            DlPtrList::Item i3 = l.insertAfter(i1, &t3);
            DlPtrList::Item i4 = l.insertBefore(i1, &t4);

            int h = i1->a;
            l.remove(i1);

            for (auto value : l) {
                cout << value->a << endl;
            }

            DlPtrList l2 = move(l);

            for (auto value : l) {
                cout << value->a << endl;
            }

            for (auto value : l2) {
                cout << value->a << endl;
            }
        }

        cout << endl;

        {
            using ObjStack = GreedyCollections::ObjStack<Test, SimpleAllocator>;
            ObjStack l(allocator, 0, 3);

            ObjStack::Item i1 = l.push(1);
            ObjStack::Item i2 = l.push(2);
            ObjStack::Item i3 = l.push(3);
            ObjStack::Item i4 = l.push(4);
            int h = i1->a;
            l.pop();

            for (auto value : l) {
                cout << value->a << endl;
            }

            ObjStack l2 = move(l);

            for (auto value : l) {
                cout << value->a << endl;
            }

            for (auto value : l2) {
                cout << value->a << endl;
            }
        }

        cout << endl;

        {
            using PtrStack = GreedyCollections::PtrStack<Test, SimpleAllocator>;
            PtrStack l(allocator, 0, 3);

            Test t1(1);
            Test t2(2);
            Test t3(3);
            Test t4(4);

            PtrStack::Item i1 = l.push(&t1);
            PtrStack::Item i2 = l.push(&t2);
            PtrStack::Item i3 = l.push(&t3);
            PtrStack::Item i4 = l.push(&t4);
            int h = i1->a;
            l.pop();

            for (auto value : l) {
                cout << value->a << endl;
            }

            PtrStack l2 = move(l);

            for (auto value : l) {
                cout << value->a << endl;
            }

            for (auto value : l2) {
                cout << value->a << endl;
            }
        }

        cout << endl;

        {
            using ObjQueue = GreedyCollections::ObjQueue<Test, SimpleAllocator>;
            ObjQueue l(allocator, 0, 3);

            auto r = l.enqueue(1);
            l.enqueue(2);
            l.enqueue(3);
            l.enqueue(4);
            int h = r->a;
            l.dequeue();

            for (auto value : l) {
                cout << value->a << endl;
            }

            ObjQueue l2 = move(l);

            for (auto value : l) {
                cout << value->a << endl;
            }

            for (auto value : l2) {
                cout << value->a << endl;
            }
        }

        cout << endl;

        {
            using PtrQueue = GreedyCollections::PtrQueue<Test, SimpleAllocator>;
            PtrQueue l(allocator, 0, 3);

            Test t1(1);
            Test t2(2);
            Test t3(3);
            Test t4(4);

            PtrQueue::Item i1 = l.enqueue(&t1);
            PtrQueue::Item i2 = l.enqueue(&t2);
            PtrQueue::Item i3 = l.enqueue(&t3);
            PtrQueue::Item i4 = l.enqueue(&t4);
            int h = i1->a;
            l.dequeue();

            for (auto value : l) {
                cout << value->a << endl;
            }

            PtrQueue l2 = move(l);

            for (auto value : l) {
                cout << value->a << endl;
            }

            for (auto value : l2) {
                cout << value->a << endl;
            }
        }

        cout << endl;

        //test();
    }
    catch (const Exception& e) {
        std::cerr << e.what() << std::endl;
    }

    if (objcount > 0) {
        cout << "leak" << endl;
    }


    cout << "done" << endl;
    return 0;
}
