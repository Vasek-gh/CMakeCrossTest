#include <iostream>
#include <signal.h>
#include <stdexcept>

using namespace std;

void signalHandler(int sig)
{
    throw runtime_error("jh");
}

void test()
{
    int* i = nullptr;
    *i = 1;
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");
    signal(SIGSEGV, signalHandler);

    cout << "start" << endl;

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

#ifdef WIN
        cout << "Windows" << endl;
#endif

#ifdef UNIX
        cout << "Unix" << endl;
#endif

#ifdef MINGW
        cout << "MinGw" << endl;
#endif


        test();
    }
    catch (...) {
        std::cerr << "exception" << std::endl;
    }

    cout << "done" << endl;    
    return 0;
}
