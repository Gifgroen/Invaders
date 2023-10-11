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
    printf("MovementDirection = (%f, %f)\n", MovementDirection.X, MovementDirection.Y);

    real32 Speed = 5 * 64; // m/s
    v2 Acceleration = MovementDirection * Speed;
    printf("Acceleration = (%f, %f)\n", Acceleration.X, Acceleration.Y);

    v2 newVelocity = Acceleration * GameState->DeltaTime + GameState->Velocity;
    printf("newVelocity = (%f, %f)\n", newVelocity.X, newVelocity.Y);

    // TODO: friction to "brake/decelerate"

    v2 NewPlayerP = 0.5f * Acceleration * Square(GameState->DeltaTime) + newVelocity * GameState->DeltaTime + GameState->PlayerPosition;
    printf("newPlayerP = (%f, %f)\n", NewPlayerP.X, NewPlayerP.Y);

    GameState->PlayerPosition = NewPlayerP;
    GameState->Velocity = newVelocity;


    u32 RectColor = 0xFF00FF00;
    printf("Drawing at (%f, %f)\n", GameState->PlayerPosition.X, GameState->PlayerPosition.Y);
    DrawRectangle(Buffer, GameState->PlayerPosition, GameState->PlayerSize, RectColor);
}
