#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");

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
    }
    catch (...) {
        std::cerr << "exception" << std::endl;
    }

    cout << "done" << endl;
    int i;
    cin >> i;
    return 0;
}
