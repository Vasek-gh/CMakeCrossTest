#include "Utils.h"
#include <vector>

using namespace std;

string getErrorMessage(DWORD err)
{
    LPSTR buffer;

    DWORD size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR) &buffer,
        0,
        NULL
    );

    string result;
    result.reserve(50);

    if (size == 0) {
        result.append("Unknown error code");
    }
    else {
        result.append(buffer, size);
    }

    result.append("(");
    result.append(to_string(err));
    result.append(")");

    LocalFree(buffer);
    return result;
}

string getLastErrorMessage()
{
    return getErrorMessage(GetLastError());
}
