#include "game.h"

#include <stdio.h>
#include <stdlib.h>

void GameUpdateAndRender(game_state *GameState, void *Pixels, int W, int H)
{
    for (int Y = 0; Y < H; ++Y)
    {
        u32 *Row = (u32*)Pixels + (Y * W);
        for (int X = 0; X < W; ++X)
        {
            u32 *Pixel = Row + X;
            *Pixel = 0xFFFF00FF;
        }
    }
}
