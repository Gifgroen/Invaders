#ifndef MATH_H
#define MATH_H

#include "types.h"

v2 V2(u32 X, u32 Y);

inline v2 operator*(real32 Lhs, v2 Rhs)
{
    v2 Result = {};
    Result.X *= Lhs * Rhs.X;
    Result.Y *= Lhs * Rhs.Y;
    return Result;
}

inline v2 operator*(v2 Lhs, real32 Rhs)
{
    return Rhs * Lhs;
}

inline v2 operator+(v2 Lhs, v2 Rhs)
{
    v2 Result = {};
    Result.X += Lhs.X + Rhs.X;
    Result.Y += Lhs.Y + Rhs.Y;
    return Result;
}

#endif // MATH_H