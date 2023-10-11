#ifndef MATH_H
#define MATH_H

#include "types.h"

v2i V2i(u32 X, u32 Y);

v2 V2(real32 X, real32 Y);

/**
 *  V2 operators
 */
inline v2 operator*(real32 Lhs, v2 Rhs)
{
    v2 Result = {};
    Result.X *= Lhs * Rhs.X;
    Result.Y *= Lhs * Rhs.Y;
    return Result;
}

inline v2 operator*(v2 Lhs, real32 Rhs)
{
    v2 Result = {};
    Result.X = Rhs * Lhs.X;
    Result.Y = Rhs * Lhs.Y;
    return Result;
}

inline v2 operator+(v2 Lhs, v2 Rhs)
{
    v2 Result = {};
    Result.X += Lhs.X + Rhs.X;
    Result.Y += Lhs.Y + Rhs.Y;
    return Result;
}

inline v2 operator*=(v2 Lhs, real32 Rhs)
{
    v2 Result = Lhs;
    Result.X *= Rhs;
    Result.Y *= Rhs;
    return Result;
}

#endif // MATH_H