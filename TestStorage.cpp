#include <iostream>
#include "TestStorage.h"

static int testCount = 0;

class TestClass
{
public:
    TestClass()
    {
        _value = 99;
        testCount++;
    }

    TestClass(const TestClass& src)
    {
        _value = src._value + 1;
    }

    ~TestClass()
    {
        _value = -1;
        testCount--;
    }
private:
    int _value;
};

struct alignas(16) TestStruct
{
    uint8_t a;
    alignas(32) uint16_t b;

    TestStruct()
    {
        a = 0;
        b = 1;
    }
};

TestStorage::TestStorage()
{

}

void TestStorage::run()
{
    {
        ObjectStorage<TestClass> storageClass;
        for (int i = 0; i < 1000; i++) {
            storageClass.create();
        }

        ObjectStorage<TestStruct> storageStruct;
        for (int i = 0; i < 1000; i++) {
            storageStruct.create();
        }
    }

    if (testCount > 0) {
        std::cout << "leak" << std::endl;
    }
}
