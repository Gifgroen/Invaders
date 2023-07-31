#ifndef GAMELIB_H
#define GAMELIB_H

#include "defs.h"
#include "game.h"
#include "os_input.h"

typedef void (*GameUpdateAndRender_t)(game_state *, offscreen_buffer *, game_input*);

struct game_lib
{
    char const *LibPath;
    void* LibHandle;
    s64 LastWriteTime;

    GameUpdateAndRender_t GameUpdateAndRender;
};

__internal int LoadGameCode(game_lib *GameCode);

#endif // GAMELIB_H