#include "game.h"

#include <stdio.h>
#include <stdlib.h>

void GameUpdateAndRender(game_state *GameState, offscreen_buffer *Buffer)
{
    v2 Size = Buffer->Size;
    for (int Y = 0; Y < Size.Height; ++Y)
    {
        u32 *Row = (u32*)Buffer->Pixels + (Y * Size.Width);
        for (int X = 0; X < Size.Width; ++X)
        {
            u32 *Pixel = Row + X;
            *Pixel = 0xFFFF00FF;
        }
    }
}
