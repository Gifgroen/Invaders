#include "game.h"

#include "math.cc"
#include "debug_io.cc"

#include <iostream>

void GameInit(game_memory *GameMemory)
{
    game_state *GameState = (game_state*)GameMemory->TransientStorage;
    std::cout << "GameInit: running = " << GameState->Running << std::endl;

    // Read defs.h and print the result
    debug_read_file_result Result = DebugReadEntireFile("../src/defs.h");
    char const *Content = (char *)Result.Content;
    std::cout << "Result: " << Content << std::endl;
    DebugFreeFileMemory(Result.Content);
}

internal_func void Clear(offscreen_buffer *Buffer, u32 Color) 
{
    v2 Size = Buffer->Size;
    for (u32 Y = 0; Y < Size.Height; ++Y)
    {
        u32 *Row = (u32*)Buffer->Pixels + (Y * Size.Width);
        for (u32 X = 0; X < Size.Width; ++X)
        {
            *(Row + X) = Color;
        }
    }
}

internal_func void DrawRectangle(offscreen_buffer *Buffer, v2 Origin, v2 Size, u32 Color) 
{   
    u32 Row = Origin.Y * Buffer->Size.Width;
    u32 Column = Origin.X;
    u32 *Target = (u32 *)Buffer->Pixels + Row + Column;
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
    game_state *GameState = (game_state*)GameMemory->TransientStorage;

    u32 Color = 0xFFFF00FF;
    Clear(Buffer, Color);

    game_controller Keyboard = Input->Keyboards[0];
    game_controller Controller = Input->Controllers[0];

    if (Keyboard.MoveLeft.IsDown || Controller.MoveLeft.IsDown)
    {
        GameState->PlayerOrigin.X *= 0.99f;
    }
    if (Keyboard.MoveRight.IsDown || Controller.MoveRight.IsDown)
    {
        GameState->PlayerOrigin.X *= 1.01f;
    }

    u32 RectColor = 0xFF00FF00;
    DrawRectangle(Buffer, GameState->PlayerOrigin, GameState->PlayerSize, RectColor);
}
