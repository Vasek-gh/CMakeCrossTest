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
#include "Collections/Holder.h"
#include "Collections/Array.h"
#include "Collections/Pool.h"
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
        objcount++;
    }

    ~Test() {
        a = 7;
        objcount--;
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
            using SlObjList = GreedyContainers::SlObjList<Test, SimpleAllocator>;
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
            using SlPtrList = GreedyContainers::SlPtrList<Test, SimpleAllocator>;
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
            using DlObjList = GreedyContainers::DlObjList<Test, SimpleAllocator>;
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
            using DlPtrList = GreedyContainers::DlPtrList<Test, SimpleAllocator>;
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
            using ObjStack = GreedyContainers::ObjStack<Test, SimpleAllocator, 2>;
            ObjStack l(allocator, 3);

            l.push(1);
            l.push(2);
            l.push(3);
            l.push(4);
            l.push(5);

            auto a1 = l.peek();
            l.pop();
            auto a2 = l.peek();
            l.pop();
            auto a3 = l.peek();
            l.pop();
            auto a4 = l.peek();
            l.pop();
        }

        cout << endl;

        {
            using PtrStack = GreedyContainers::PtrStack<Test, SimpleAllocator, 2>;
            PtrStack l(allocator, 3);

            Test t1(1);
            Test t2(2);
            Test t3(3);
            Test t4(4);

            l.push(&t1);
            l.push(&t2);
            l.push(&t3);
            l.push(&t4);
            l.pop();
        }

        cout << endl;

        {
            using ObjQueue = GreedyContainers::ObjQueue<Test, SimpleAllocator, 2>;
            ObjQueue l(allocator, 3);

            auto r = l.enqueue(1);
            l.enqueue(2);
            l.enqueue(3);
            l.enqueue(4);
            l.enqueue(5);
            l.dequeue();
            l.dequeue();
            l.dequeue();
            l.dequeue();
            l.dequeue();
            l.enqueue(6);
        }

        cout << endl;

        {
            using PtrQueue = GreedyContainers::PtrQueue<Test, SimpleAllocator, 2>;
            PtrQueue l(allocator, 3);

            Test t1(1);
            Test t2(2);
            Test t3(3);
            Test t4(4);

            l.enqueue(&t1);
            l.enqueue(&t2);
            l.enqueue(&t3);
            l.enqueue(&t4);
            l.dequeue();

        }

        cout << endl;

        {
            using Array = GreedyContainers::Array<Test>;
            using ArrayBuilder = GreedyContainers::ArrayBuilder<Test, SimpleAllocator>;

            using ObjHolder = GreedyContainers::ObjHolder<Test, SimpleAllocator, 2>;
            ObjHolder l(allocator, 3);

            auto r = l.add(1);
            l.add(2);
            l.add(3);

            ArrayBuilder builder(allocator, l.count());
            auto e1 = l.getEnumerator();
            while (e1.moveNext()) {
                builder.add(e1.current());
                cout << e1.current()->a << endl;
            }

            auto arr = l.toArray();
            for (auto value : arr) {
                cout << value->a << endl;
            }

            l.clear();

            auto arr2 = l.toArray();
            for (auto value : arr2) {
                cout << value->a << endl;
            }

            auto e2 = l.getEnumerator();
            while (e2.moveNext()) {
                cout << e2.current()->a << endl;
            }
        }

        cout << endl;

        {
            using PtrHolder = GreedyContainers::PtrHolder<Test, SimpleAllocator, 2>;
            PtrHolder l(allocator, 3);

            Test t1(1);
            Test t2(2);
            Test t3(3);
            Test t4(4);

            l.add(&t1);
            l.add(&t2);
            l.add(&t3);
            l.add(&t4);

            auto e1 = l.getEnumerator();
            while (e1.moveNext()) {
                cout << e1.current()->a << endl;
            }


            l.clear();

            auto e2 = l.getEnumerator();
            while (e2.moveNext()) {
                cout << e2.current()->a << endl;
            }


            /*for (auto value : l) {
                cout << value->a << endl;
            }

            PtrQueue l2 = move(l);

            for (auto value : l) {
                cout << value->a << endl;
            }

            for (auto value : l2) {
                cout << value->a << endl;
            }*/
        }

        cout << endl;

        {
            using Pool = GreedyContainers::Pool<Test, SimpleAllocator, 2>;
            Pool l(allocator, 3);

            {
                auto r = l.create(1);
                {
                    auto r2 = l.create(2);

                    {
                        auto r3 = l.create(3);
                    }
                }
            }

            auto r4 = l.create(4);
        }

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
