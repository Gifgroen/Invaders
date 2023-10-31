#include "math.h"

/**
 * Constants
 */
real32 Pi32 = 3.141592653589793f;

/**
 * General
 */
real64 Square(real64 Factor)
{
    real64 Result = Factor * Factor;
    return Result;
}

real32 Dot(v2 A, v2 B)
{
    real32 Result = A.x * B.x + A.y * B.y;
    return Result;
}

v2 Perp(v2 A)
{
    v2 Result = V2(-A.y, A.x);
    return Result;
}

/**
 *  V2
 */
v2i V2i(u32 X, u32 Y)
{
    v2i Result = {};
    Result.x = X;
    Result.y = Y;
    return Result;
}

v2 V2(real32 X, real32 Y)
{
    v2 Result = {};
    Result.x = X;
    Result.y = Y;
    return Result;
}

/**
 *  V2 operators
 */
inline v2 operator-(v2 A) 
{
    v2 Result;
    Result.x = -A.x;
    Result.y = -A.y;
    return Result;
}

inline v2 operator*(real32 Lhs, v2 Rhs)
{
    v2 Result = {};
    Result.x = Lhs * Rhs.x;
    Result.y = Lhs * Rhs.y;
    return Result;
}

inline v2 operator*(v2 Lhs, real32 Rhs)
{
    v2 Result = {};
    Result.x = Rhs * Lhs.x;
    Result.y = Rhs * Lhs.y;
    return Result;
}

inline v2& operator*=(v2 &Lhs, real32 Rhs)
{
    Lhs = Lhs * Rhs;
    return Lhs;
}

inline v2 operator+(v2 Lhs, v2 Rhs)
{
    v2 Result = {};
    Result.x = Lhs.x + Rhs.x;
    Result.y = Lhs.y + Rhs.y;
    return Result;
}

inline v2& operator+=(v2 &Lhs, v2 Rhs)
{
    Lhs = Lhs + Rhs;
    return Lhs;
}

inline v2 operator-(v2 Lhs, v2 Rhs)
{
    v2 Result = {};
    Result.x = Lhs.x - Rhs.x;
    Result.y = Lhs.y - Rhs.y;
    return Result;
}

inline v2& operator-=(v2 &Lhs, v2 Rhs)
{
    Lhs = Lhs - Rhs;
    return Lhs;
}
