#ifndef GAME_H
#define GAME_H

#include "defs.h"
#include "types.h"
#include "math.h"
#include "os_input.h"

struct offscreen_buffer 
{
    v2 Size;
    void *Pixels;
};

struct game_state 
{
    bool Running;
};

struct game_memory
{
    u64 TransientStorageSize;
    void *TransientStorage;

    u64 PermanentStorageSize;
    void *PermanentStorage;
};

extern "C" 
{
    #if defined(PLATFORM_WIN) 
    __declspec(dllexport) 
    #endif
    void GameUpdateAndRender(game_memory *GameMemory, offscreen_buffer *Buffer, game_input *Input);

    #if defined(PLATFORM_WIN) 
    __declspec(dllexport) 
    #endif
    void GameInit(game_memory *GameMemory);
}

#endif