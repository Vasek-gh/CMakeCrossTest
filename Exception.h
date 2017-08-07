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
    explicit Exception(CharStr msg);
    explicit Exception(CharStr file, int line, StdStr msg);
    explicit Exception(CharStr file, int line, CharStr msg);
    virtual ~Exception();

    virtual const char* what() const throw() override;
protected:
    explicit Exception(CharStr EName, CharStr file, int line, StdStr msg);
private:
    std::string makeFullMessage(CharStr EName, CharStr file, int line, StdStr msg);
    std::string makeFullMessage(CharStr EName, CharStr file, int line, CharStr msg);

    // full message of exception
    const std::string _msg;
};

//##############################################################################
//
// RuntimeException
//
//##############################################################################

class RuntimeException : public Exception
{
public:
    explicit RuntimeException(StdStr msg);
    explicit RuntimeException(CharStr file, int line, StdStr msg);
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
protected:
    using Exception::Exception;
};

//##############################################################################
//
// ArgumentNullException
//
//##############################################################################

class ArgumentNullException : public ArgumentException
{
public:
    explicit ArgumentNullException(StdStr msg);
    explicit ArgumentNullException(CharStr file, int line, StdStr msg);
};

//##############################################################################
//
// BadAllocException
//
//##############################################################################

class BadAllocException : public Exception
{
public:
    explicit BadAllocException(StdStr msg);
    explicit BadAllocException(CharStr file, int line, StdStr msg);
};

//##############################################################################
//
// NullPointerException
//
//##############################################################################

class NullPointerException : public Exception
{
public:
    explicit NullPointerException(StdStr msg);
    explicit NullPointerException(CharStr file, int line, StdStr msg);
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

//##############################################################################
//
// Utils
//
//##############################################################################

#define CHECK_NULL_PTR(ptr) \
    if (!(ptr)) { \
        Exceptions::Internal::raiseNullPointerException( \
            __FILE__, __LINE__, #ptr \
        ); \
    } \

#define CHECK_NULL_ARG(arg) \
    if (!(arg)) { \
        Exceptions::Internal::raiseArgumentNullException( \
            __FILE__, __LINE__, #arg \
        ); \
    } \


namespace Exceptions {
    namespace Internal {
        using CharStr = const char*;

        void raiseNullPointerException(CharStr f, int l, CharStr name);
        void raiseArgumentNullException(CharStr f, int l, CharStr name);
    }
}


#endif // EXCEPTION_H
