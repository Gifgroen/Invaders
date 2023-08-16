#include "game.h"

#include "math.cc"

#include <iostream>

void GameInit(game_memory *GameMemory)
{
    game_state *GameState = (game_state*)GameMemory->TransientStorage;
    std::cout << "GameInit: running = " << GameState->Running << std::endl;
}

internal_func void Clear(offscreen_buffer *Buffer, u32 Color) 
{
    v2 Size = Buffer->Size;
    for (u32 Y = 0; Y < Size.Height; ++Y)
    {
        u32 *Row = (u32*)Buffer->Pixels + (Y * Size.Width);
        for (u32 X = 0; X < Size.Width; ++X)
        {
            u32 *Pixel = Row + X;

            *Pixel = Color;
        }
    }   
}

internal_func void DrawRectangle(offscreen_buffer *Buffer, v2 Origin, v2 Size, u32 Color) 
{   
    u32 *Pixels = (u32 *)Buffer->Pixels;

    u32 Row = Origin.Y * Buffer->Size.Width;
    u32 Column = Origin.X;
    u32 *Target = Pixels + Row + Column;
    for (s32 Y = 0; Y < Size.Height; ++Y)
    {
        for (s32 X = 0; X < Size.Width; ++X)
        {
            *(Target + X) = Color;
        }
        Target += Buffer->Size.Width;
    }
}

void GameUpdateAndRender(game_memory *GameMemory, offscreen_buffer *Buffer, game_input *Input)
{
    // game_state *GameState = (game_state*)GameMemory->TransientStorage;

    u32 Color = 0xFFFF00FF;
    Clear(Buffer, Color);

    v2 Origin = V2(16, 16);
    v2 Size = V2(64, 64);
    u32 RectColor = 0xFF00FF00;
    DrawRectangle(Buffer, Origin, Size, RectColor);
}
