#ifndef GAMELIB_H
#define GAMELIB_H

#include "defs.h"
#include "game.h"
#include "os_input.h"

typedef void (*GameInit_t)(game_memory *);
typedef void (*GameUpdateAndRender_t)(game_memory *, offscreen_buffer *, game_input*);

struct game_lib
{
    char const *LibPath;
    void* LibHandle;
    s64 LastWriteTime;

    GameInit_t GameInit;
    GameUpdateAndRender_t GameUpdateAndRender;
};

internal_func int LoadGameCode(game_lib *GameCode);

#endif // GAMELIB_H