#ifndef GAME_H
#define GAME_H

#include "defs.h"
#include "math.cc"
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

extern "C" 
{
    void GameUpdateAndRender(game_state *GameState, offscreen_buffer *Buffer, game_input *Input);
}

#endif