#include "Utils.h"
#include <vector>
#include <errno.h>
#include <string.h>

using namespace std;

string getErrorMessage(int err)
{
    const int BUFFER_SIZE = 1024;

    char buffer[BUFFER_SIZE];
    char* str = strerror_r(err, &buffer[0], BUFFER_SIZE);

    string result;
    result.append(str);
    return result;
}

string getLastErrorMessage()
{
    return getErrorMessage(errno);
}
