#define global_variable static
#define internal_func static
#define local_persist static

#define ArrayCount(Array) (sizeof(Array)/sizeof(*(Array)))

#if DEBUG
#if defined(PLATFORM_MACOS)
#define Assert(Expression) if (!(Expression)) { __builtin_trap(); }
#elif defined(PLATFORM_WIN)
#define Assert(Expression) if (!(Expression)) { *(int *)0 = 0; }
#endif
#else
#define Assert(Expression)
#endif

#define Kilobytes(Value) (Value * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)
