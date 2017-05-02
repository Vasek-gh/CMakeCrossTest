#ifndef DEBUG_H
#define DEBUG_H

#define ASSERT_1(condition) \
    if (!(condition)) { \
        Debug::Private::raiseAssert(__FILE__, __LINE__, #condition); \
    } \

#define ASSERT_2(condition, msg) \
    if (!(condition)) { \
        Debug::Private::raiseAssert(__FILE__, __LINE__, #condition, msg); \
    } \

#define _ARG3(arg1, arg2, arg3, ...) arg3
#define ASSERT_SELECT(...) _ARG3(__VA_ARGS__, ASSERT_2, ASSERT_1)
#define ASSERT(...) ASSERT_SELECT(__VA_ARGS__)(__VA_ARGS__)


#define CHECK_NULL_ARG(arg) \
    Debug::Private::isPointer(arg); \
    Debug::Private::checkNullArg(__FILE__, __LINE__, arg, #arg);

#include <type_traits>

namespace Debug {
    namespace Private {
        using CharStr = const char*;

        // helper for check that argument is pointer
        template <class T>
        void isPointer(const T& src) {
            static_assert(
                std::is_pointer<T>::value,
                "Parameter must be a pointer type"
            );
        }

        void raiseAssert(CharStr f, int l, CharStr condition);
        void raiseAssert(CharStr f, int l, CharStr condition, CharStr msg);
        void checkNullArg(CharStr f, int l, void* arg, CharStr name);
    }
}

#endif // DEBUG_H
