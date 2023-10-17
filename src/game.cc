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
    v2i Size = Buffer->Size;
    for (u32 Y = 0; Y < Size.Height; ++Y)
    {
        u32 *Row = (u32*)Buffer->Pixels + (Y * Size.Width);
        for (u32 X = 0; X < Size.Width; ++X)
        {
            *(Row + X) = Color;
        }
    }
}

internal_func void DrawRectangle(offscreen_buffer *Buffer, v2 Origin, v2i Size, u32 Color) 
{
    // TODO: properly round Origin.X and Origin.Y.
    u32 Row = (u32)Origin.Y * Buffer->Size.Width;
    u32 Column = (u32)Origin.X;

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

real64 Square(real64 Factor)
{
    real64 Result = Factor * Factor;
    return Result;
}

void GameUpdateAndRender(game_memory *GameMemory, offscreen_buffer *Buffer, game_input *Input)
{
    game_state *GameState = (game_state*)GameMemory->TransientStorage;

    u32 Color = 0xFFFF00FF;
    Clear(Buffer, Color);

    game_controller Keyboard = Input->Keyboards[0];
    game_controller Controller = Input->Controllers[0];

    v2 MovementDirection = {};
    if (Keyboard.MoveLeft.IsDown || Controller.MoveLeft.IsDown)
    {
        MovementDirection.X = -1.f;
    }
    if (Keyboard.MoveRight.IsDown || Controller.MoveRight.IsDown)
    {
        MovementDirection.X = 1.f;
    }
    if (Keyboard.MoveUp.IsDown || Controller.MoveUp.IsDown)
    {
        MovementDirection.Y = -1.f;
    }
    if (Keyboard.MoveDown.IsDown || Controller.MoveDown.IsDown)
    {
        MovementDirection.Y = 1.f;
    }

    if (MovementDirection.X != 0.f && MovementDirection.Y != 0.f)
    {
        MovementDirection *= 0.707106f;
    }

    int PixelsPerMeter = 64;

    // TODO: do this adjustment later.
    real32 Speed = 5 * PixelsPerMeter; // m/s * pixelsPerMeter = pixels per meter / s
    v2 Acceleration = MovementDirection * Speed;

    v2 newVelocity = Acceleration * GameState->DeltaTime + GameState->Velocity;

    // TODO: friction to "brake/decelerate"

    v2 NewPlayerP = 0.5f * Acceleration * Square(GameState->DeltaTime) + newVelocity * GameState->DeltaTime + GameState->PlayerPosition;
    
    GameState->PlayerPosition = NewPlayerP;
    GameState->Velocity = newVelocity;

    u32 RectColor = 0xFF00FF00;
    DrawRectangle(Buffer, GameState->PlayerPosition, GameState->PlayerSize, RectColor);
}
