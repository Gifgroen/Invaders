#ifndef DEBUG_IO_H
#define DEBUG_IO_H

#include "defs.h"
#include "types.h"

struct debug_read_file_result
{
    u64 ContentSize;
    void *Content;
};

#endif // DEBUG_IO_H
