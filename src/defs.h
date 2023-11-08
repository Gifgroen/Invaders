#define global_variable static
#define internal_func static
#define local_persist static

#define ArrayCount(Array) (sizeof(Array)/sizeof(*(Array)))

#if DEBUG
#define Assert(Expression) if (!(Expression)) { __builtin_trap(); }
#else
#define Assert(Expression)
#endif

#define Kilobytes(Value) (Value * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)
