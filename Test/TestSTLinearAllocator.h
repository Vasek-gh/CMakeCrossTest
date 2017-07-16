#ifndef TESTSTLINEARALLOCATOR_H
#define TESTSTLINEARALLOCATOR_H


class TestSTLinearAllocator
{
public:
    TestSTLinearAllocator();

    void run();
private:
    void speedTest();
    void speedTestThreads();
};

#endif // TESTSTLINEARALLOCATOR_H
