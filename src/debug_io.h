#ifndef DEBUG_IO_H
#define DEBUG_IO_H

#include "defs.h"
#include "types.h"

struct debug_read_file_result
{
    u64 ContentSize;
    void *Content;
};

internal_func void DebugFreeFileMemory(void *Memory);

internal_func debug_read_file_result DebugReadEntireFile(char const *Filename);

internal_func bool DebugWriteEntireFile(char const *Filename, char const *Content, u64 Length);

#endif // DEBUG_IO_H
