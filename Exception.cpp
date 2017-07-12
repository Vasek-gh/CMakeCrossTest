#include "Exception.h"

using namespace std;

//##########################################
//
// Exception
//
//##########################################d

Exception::Exception(StdStr msg)
    : _msg(makeFullMessage("Exception", "", -1, msg))
{
}

Exception::Exception(CharStr file, int line, StdStr msg)
    : _msg(makeFullMessage("Exception", file, line, msg))
{
}

Exception::~Exception()
{
}

const char* Exception::what() const throw()
{
    return _msg.c_str();
}

Exception::Exception(CharStr EName, CharStr file, int line, StdStr msg)
    : _msg(makeFullMessage(EName, file, line, msg))
{

}

string Exception::makeFullMessage(CharStr EName, CharStr file, int line,
                                  StdStr msg)
{
    string result;
    result.reserve(msg.size() + 100);
    result.append(EName);
    if (line > -1) {
        result.append(" at ");
        result.append(file);
        result.append("[");
        result.append(std::to_string(line));
        result.append("]");
        result.append(": ");
    }
    else {
        result.append(": ");
    }

    result.append(msg);
    return result;
}

//##########################################
//
// RuntimeException
//
//##########################################

RuntimeException::RuntimeException(Exception::StdStr msg)
    : Exception("ArgumentException", "", -1, msg)
{
}

RuntimeException::RuntimeException(Exception::CharStr file, int line,
                                   Exception::StdStr msg)
    : Exception("ArgumentException", file, line, msg)
{
}

//##########################################
//
// ArgumentException
//
//##########################################

ArgumentException::ArgumentException(StdStr msg)
    : Exception("ArgumentException", "", -1, msg)
{
}

ArgumentException::ArgumentException(Exception::CharStr file, int line,
                                     Exception::StdStr msg)
    : Exception("ArgumentException", file, line, msg)
{
}

//##########################################
//
// BadAllocException
//
//##########################################

BadAllocException::BadAllocException(Exception::StdStr msg)
    : Exception("BadAllocException", "", -1, msg)
{
}

BadAllocException::BadAllocException(Exception::CharStr file, int line,
                                     Exception::StdStr msg)
    : Exception("BadAllocException", file, line, msg)
{
}
