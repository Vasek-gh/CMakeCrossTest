#ifndef GCOLLECTIONS_H
#define GCOLLECTIONS_H

#include <stdint.h>

namespace GreedyCollections {
namespace Internal {

    // минимальный размер пачки
    const static size_t MIN_CHUNK_SIZE = 3;
    // максимальный размер пачки
    const static size_t MAX_CHUNK_SIZE = 50;

    namespace Errors {
        const char* UnknownNode =
                "Node not exist in list";
        const char* EmptyContainer =
                "Container is empty";
    }
}
}

#endif // GCOLLECTIONS_H
