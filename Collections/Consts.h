#ifndef GCOLLECTIONS_H
#define GCOLLECTIONS_H

#include <stdint.h>

namespace GreedyContainers {
namespace Internal {

    // минимальный размер пачки
    const static size_t MIN_CHUNK_SIZE = 3;
    // максимальный размер пачки
    const static size_t MAX_CHUNK_SIZE = 50;

    // размер предварительной пачки
    const static size_t DEF_CAPACITY = 3;
    // размер пачки по умолчанию
    const static size_t DEF_CHUNK_SIZE = 5;


    namespace Errors {
        static const char* UnknownNode =
                "Node not exist in list";
        static const char* EmptyContainer =
                "Container is empty";
        static const char* FullContainer =
                "Container is full";
        static const char* MovedContainer =
                "Container is moved";
        static const char* IndexOutOfRange =
                "Index out of range";
    }
}
}

#endif // GCOLLECTIONS_H
