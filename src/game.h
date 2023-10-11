#ifndef GAME_H
#define GAME_H

#include "defs.h"
#include "types.h"
#include "os_input.h"

struct offscreen_buffer 
{
    v2 Size;
    void *Pixels;
};

struct game_state 
{
    bool Running;
    real64 DeltaTime;

    v2 PlayerPosition;
    v2 PlayerSize;

    v2 Velocity;
};

struct game_memory
{
    u64 TransientStorageSize;
    void *TransientStorage;

    u64 PermanentStorageSize;
    void *PermanentStorage;
};

typedef void (*GameInit_t)(game_memory *);
typedef void (*GameUpdateAndRender_t)(game_memory *, offscreen_buffer *, game_input*);

extern "C" 
{
    #if defined(PLATFORM_WIN) 
    __declspec(dllexport) 
    #endif
    void GameInit(game_memory *GameMemory);

    #if defined(PLATFORM_WIN) 
    __declspec(dllexport) 
    #endif
    void GameUpdateAndRender(game_memory *GameMemory, offscreen_buffer *Buffer, game_input *Input);
}

#endif // GAME_H