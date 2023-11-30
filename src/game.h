#ifndef GAME_H
#define GAME_H

#include "defs.h"
#include "types.h"

#include "assets.h"
#include "memory.h"
#include "os_input.h"

struct offscreen_buffer 
{
    v2i Size;
    void *Pixels;
};

struct game_state 
{
    bool Running;
    real64 DeltaTime;

    memory_journal Journal;

    v2 PlayerPosition;
    v2i PlayerSize;
    v2 Velocity;

    real64 ElapsedTime;

    int ToneHz;

    assets *Assets;
};

struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    s16 *Samples;
};

struct game_memory
{
    u64 PermanentStorageSize;
    void *PermanentStorage;

    u64 TransientStorageSize;
    void *TransientStorage;

    char const *BasePath;
};

typedef void (*GameInit_t)(game_memory *, offscreen_buffer *);
typedef void (*GameUpdateAndRender_t)(game_memory *, offscreen_buffer *, game_input*);
typedef void (*GameGetSoundSamples_t)(game_memory *, game_sound_output_buffer *);

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

    #if defined(PLATFORM_WIN)
    __declspec(dllexport)
    #endif
    void GameGetSoundSamples(game_memory *GameMemory, game_sound_output_buffer *SoundBuffer);
}

#endif // GAME_H