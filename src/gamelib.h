#ifndef GAMELIB_H
#define GAMELIB_H

#include "defs.h"
#include "game.h"

typedef void (*GameUpdateAndRender_t)(game_state *, void *, int, int);

struct game_lib
{
    char const *LibPath;
    void* LibHandle;
    s64 LastWriteTime;

    GameUpdateAndRender_t GameUpdateAndRender;
};

internal int LoadGameCode(game_lib *GameCode);

#endif // GAMELIB_H