#include "game.h"

#include <stdio.h>
#include <stdlib.h>

void GameUpdateAndRender(game_state *GameState, offscreen_buffer *Buffer)
{
    for (int Y = 0; Y < Buffer->Height; ++Y)
    {
        u32 *Row = (u32*)Buffer->Pixels + (Y * Buffer->Width);
        for (int X = 0; X < Buffer->Width; ++X)
        {
            u32 *Pixel = Row + X;
            *Pixel = 0xFFFF00FF;
        }
    }
}
