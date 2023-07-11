#ifndef MATH_H
#define MATH_H

#include "defs.h"

struct v2 
{
    union 
    {
        struct
        {
            u32 X, Y;
        };
        struct 
        {
            u32 Width, Height;
        };
    };
};

#endif // MATH_H