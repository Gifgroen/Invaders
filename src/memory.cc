#include "memory.h"

#define PushStruct(Journal, Type) (Type *)_PushStruct(Journal, sizeof(Type))
#define PushArray(Journal, Type, Size) (Type *)_PushStruct(Journal, Size * sizeof(Type))
#define PushSize(Journal, Size) _PushStruct(Journal, Size)

internal_func void *_PushStruct(memory_journal *Journal, memory_size Size)
{
    Assert(Journal->Used + Size <= Journal->Size);

    void *Result = Journal->Base + Journal->Used;
    Journal->Used += Size;
    return Result;
}

internal_func void InitialiseJournal(memory_journal *Journal, u8 *Base, u64 Size)
{
    Journal->Base = Base;
    Journal->Size = Size;
    Journal->Used = 0;
}