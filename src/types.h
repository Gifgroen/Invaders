#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float real32;
typedef double real64;

struct v2 
{
    union 
    {
        struct
        {
            s32 X, Y;
        };
        struct 
        {
            s32 Width, Height;
        };
    };
};

#endif // TYPES_H
