#ifndef GAME_H
#define GAME_H

#include "defs.h"

struct game_state 
{
    bool Running;
};

extern "C" 
{
    void GameUpdateAndRender(game_state *GameState, void *Pixels, int W, int H);
}

#endif