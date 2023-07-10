#ifndef GAME_H
#define GAME_H

#include "defs.h"

struct offscreen_buffer 
{
    s16 Width; 
    s16 Height;
    void *Pixels;
};

struct game_state 
{
    bool Running;
};

extern "C" 
{
    void GameUpdateAndRender(game_state *GameState, offscreen_buffer *Buffer);
}

#endif