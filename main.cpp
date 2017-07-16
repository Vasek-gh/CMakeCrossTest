﻿#include <iostream>
#include <signal.h>
#include <stdexcept>
#include <chrono>
#include "Debug.h"
#include "Memory.h"
#include "Exception.h"

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

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");

    wstring ggg = L"ЫЫЫЫ";

    wcout << ggg << endl;

    signal(SIGSEGV, signalHandler);

    cout << "start" << endl;

    Memory::init();

    testEnv();
    testAssert();
    testStorage();
    testSTLinearAllocator();

    try
    {
        //test();
    }
    catch (...) {
        std::cerr << "exception" << std::endl;
    }

    if (objcount > 0) {
        cout << "leak" << endl;
    }


    cout << "done" << endl;
    return 0;
}
