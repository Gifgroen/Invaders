#ifndef MATH_H
#define MATH_H

#include "types.h"

inline real64 Square(real64 Factor);

inline real32 Dot(v2 A, v2 B);

inline v2 Perp(v2 A);

inline real32 LengthSq(v2 A);

v2i V2i(u32 X, u32 Y);

v2 V2(real32 X, real32 Y);

inline v2 operator-(v2 A);
inline v2 operator*(real32 Lhs, v2 Rhs);

inline v2 operator*(v2 Lhs, real32 Rhs);
inline v2& operator*=(v2 &Lhs, real32 Rhs);

inline v2 operator+(v2 Lhs, v2 Rhs);
inline v2& operator+=(v2 &Lhs, v2 Rhs);

inline v2 operator-(v2 Lhs, v2 Rhs);
inline v2& operator-=(v2 &Lhs, v2 Rhs);
 
#endif // MATH_H
