#include "TestSTLinearAllocator.h"

#include <iostream>
#include <chrono>
#include <thread>
#include "../LinearAllocator.h"

//#define CHECK_RESULT

using namespace std;
using namespace std::chrono;
using Time = std::chrono::high_resolution_clock::time_point;

class TestClass
{
public:
    TestClass()
    {
        _value = 99;
    }

    TestClass(const TestClass& src)
    {
        _value = src._value + 1;
    }

    ~TestClass()
    {
        _value = -1;
    }

    void* operator new (size_t size, STLinearAllocator* allocator)
    {
        return allocator->alloc(size);
    }

    void setValue(int value)
    {
        _value = value;
    }
private:
    int _value;
};

static int count = 1000000;
static int size = 16;
static bool cleansable = false;

void threadRunLinear() {

    STLinearAllocator allocator(cleansable);
    for (int i = 0; i < count; i++) {
        uint64_t* value = reinterpret_cast<uint64_t*>(allocator.alloc(size));
        *value = 10;
    }
}

void threadRunMalloc() {

    for (int i = 0; i < count; i++) {
        uint64_t* value = reinterpret_cast<uint64_t*>(malloc(size));
        *value = 10;
    }
}


TestSTLinearAllocator::TestSTLinearAllocator()
{

}

void TestSTLinearAllocator::run()
{
    int count = 1000;

    STLinearAllocator allocator( cleansable);
    for (int i = 0; i < count; i++) {
        auto t = new (&allocator) TestClass();
        t->setValue(i);
    }

    speedTest();
    speedTestThreads();
}

void TestSTLinearAllocator::speedTest()
{
    {
        Time startTime = high_resolution_clock::now();

        STLinearAllocator allocator(cleansable);

        for (int i = 0; i < count; i++) {
            uint64_t* value = reinterpret_cast<uint64_t*>(allocator.alloc(size));
            *value = 10;
        }

        Time endTime = high_resolution_clock::now();
        cout << "ellapsed: " << duration_cast<milliseconds>(endTime - startTime).count() << endl;

#ifdef CHECK_RESULT
        int i;
        cin >> i;
#endif
    }

    {
        Time startTime = high_resolution_clock::now();

        for (int i = 0; i < count; i++) {
            uint64_t* value = reinterpret_cast<uint64_t*>(malloc(size));
            *value = 10;
        }

        Time endTime = high_resolution_clock::now();
        cout << "ellapsed: " << duration_cast<milliseconds>(endTime - startTime).count() << endl;

#ifdef CHECK_RESULT
        int i;
        cin >> i;
#endif
    }
}

void TestSTLinearAllocator::speedTestThreads()
{
    {
        Time startTime = high_resolution_clock::now();

        thread th1(&threadRunLinear);
        thread th2(&threadRunLinear);
        thread th3(&threadRunLinear);
        thread th4(&threadRunLinear);
#ifdef MINGW
        thread th5(&threadRunLinear);
        thread th6(&threadRunLinear);
        thread th7(&threadRunLinear);
        thread th8(&threadRunLinear);
#endif

        th1.join();
        th2.join();
        th3.join();
        th4.join();
#ifdef MINGW
        th5.join();
        th6.join();
        th7.join();
        th8.join();
#endif

        Time endTime = high_resolution_clock::now();
        cout << "ellapsed: " << duration_cast<milliseconds>(endTime - startTime).count() << endl;

#ifdef CHECK_RESULT
        int i;
        cin >> i;
#endif
    }

    {
        Time startTime = high_resolution_clock::now();

        thread th1(&threadRunMalloc);
        thread th2(&threadRunMalloc);
        thread th3(&threadRunMalloc);
        thread th4(&threadRunMalloc);
#ifdef MINGW
        thread th5(&threadRunMalloc);
        thread th6(&threadRunMalloc);
        thread th7(&threadRunMalloc);
        thread th8(&threadRunMalloc);
#endif

        th1.join();
        th2.join();
        th3.join();
        th4.join();
#ifdef MINGW
        th5.join();
        th6.join();
        th7.join();
        th8.join();
#endif

        Time endTime = high_resolution_clock::now();
        cout << "ellapsed: " << duration_cast<milliseconds>(endTime - startTime).count() << endl;

#ifdef CHECK_RESULT
        int i;
        cin >> i;
#endif
    }
}
