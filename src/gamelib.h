#ifndef GAMELIB_H
#define GAMELIB_H

#include "game.h"

struct game_lib
{
    char const *LibPath;
    void* LibHandle;
    s64 LastWriteTime;

    GameInit_t GameInit;
    GameUpdateAndRender_t GameUpdateAndRender;
    GameGetSoundSamples_t GameGetSoundSamples;
};

internal_func int LoadGameCode(game_lib *GameCode);

#endif // GAMELIB_H