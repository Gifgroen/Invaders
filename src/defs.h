#define global_variable static
#define internal_func static
#define local_persist static

#define ArrayCount(Array) (sizeof(Array)/sizeof(*(Array)))

#define Kilobytes(Value) Value * 1024LL
#define Megabytes(Value) Kilobytes(Value) * 1024LL
#define Gigabytes(Value) Megabytes(Value) * 1024LL
