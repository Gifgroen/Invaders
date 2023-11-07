#ifndef GAME_H
#define GAME_H

#include "defs.h"
#include "types.h"
#include "os_input.h"

#include "assets.h"

struct offscreen_buffer 
{
    v2i Size;
    void *Pixels;
};

struct game_state 
{
    bool Running;
    real64 DeltaTime;

    v2 PlayerPosition;
    v2i PlayerSize;

    v2 Velocity;

    real64 ElapsedTime;

    // Assets
    loaded_texture *Ships[3];
};

struct game_memory
{
    u64 TransientStorageSize;
    void *TransientStorage;

    u64 PermanentStorageSize;
    void *PermanentStorage;
};

typedef void (*GameInit_t)(game_memory *, offscreen_buffer *);
typedef void (*GameUpdateAndRender_t)(game_memory *, offscreen_buffer *, game_input*);

extern "C" 
{
    #if defined(PLATFORM_WIN) 
    __declspec(dllexport) 
    #endif
    void GameInit(game_memory *GameMemory, offscreen_buffer *Buffer);

    #if defined(PLATFORM_WIN) 
    __declspec(dllexport) 
    #endif
    void GameUpdateAndRender(game_memory *GameMemory, offscreen_buffer *Buffer, game_input *Input);
}

#endif // GAME_H