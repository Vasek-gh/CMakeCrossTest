#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <stdexcept>

//##############################################################################
//
// Exception
//
//##############################################################################

class Exception : public std::exception
{
protected:
    using StdStr = const std::string&;
    using CharStr = const char*;
public:
    explicit Exception(StdStr msg);
    explicit Exception(CharStr file, int line, StdStr msg);
    virtual ~Exception();

    virtual const char* what() const throw() override;
protected:
    explicit Exception(CharStr EName, CharStr file, int line, StdStr msg);
private:
    std::string makeFullMessage(CharStr EName, CharStr file, int line, StdStr msg);

    // full message of exception
    const std::string _msg;
};

//##############################################################################
//
// ArgumentException
//
//##############################################################################

class ArgumentException : public Exception
{
public:
    explicit ArgumentException(StdStr msg);
    explicit ArgumentException(CharStr file, int line, StdStr msg);
};

//##############################################################################
//
// RAISE
//
//##############################################################################

template<typename T, typename ...Args>
void raiseT(Args&&... args)
{
    throw T(std::forward<Args>(args)...);
}

#define RAISE(E, ...) \
    raiseT<E>(__FILE__, __LINE__, __VA_ARGS__)


#endif // EXCEPTION_H
