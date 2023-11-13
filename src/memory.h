#ifndef MEMORY_H
#define MEMORY_H

#include "defs.h"
#include "types.h"

struct memory_journal
{
    u8 *Base;
    memory_size Size;

    memory_size Used;
};

#endif // MEMORY_H