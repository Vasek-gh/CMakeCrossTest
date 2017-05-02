#include "Debug.h"
#include <iostream>
#include "Exception.h"

using namespace std;

namespace Debug {
namespace Private {

class AssertException : public Exception
{
public:
    explicit AssertException(CharStr file, int line, StdStr msg)
        : Exception("AssertException", file, line, msg)
    {}
};

void raiseAssert(CharStr f, int l, CharStr condition)
{
    throw AssertException(f, l, "Assertion (%1%) failed");
    /*throw AssertException(f, l, (
            boost::format("Assertion (%1%) failed")
                % condition
        ).str()
    );*/
}

void raiseAssert(CharStr f, int l, CharStr condition, CharStr msg)
{
    throw AssertException(f, l, "Assertion (%1%) failed with message");
    /*throw AssertException(f, l, (
            boost::format("Assertion (%1%) failed with message: %2%")
                % condition % msg
        ).str()
    );*/
}

void checkNullArg(CharStr f, int l, void* arg, CharStr name)
{
    if (!arg) {
        throw ArgumentException(f, l, "can not be null");
        /*throw ArgumentException(f, l, (
                boost::format("%1% can not be null")
                    % name
            ).str()
        );*/
    }
}

}
}


