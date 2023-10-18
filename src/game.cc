#include "game.h"

#include "math.cc"
#include "debug_io.cc"

#include <iostream>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

struct loaded_texture 
{
    v2 Size;
    void *Pixels;
};

void GameInit(game_memory *GameMemory)
{
    game_state *GameState = (game_state*)GameMemory->TransientStorage;
    std::cout << "GameInit: running = " << GameState->Running << std::endl;
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

internal_func void DrawTexture(offscreen_buffer *Buffer, v2 Origin, loaded_texture *Texture)
{
    // Assert(Origin.x < Origin.x + Texture->Size.Width);
    // Assert(Origin.y < Origin.y + Texture->Size.Height);

    u32 Width = Texture->Size.Width;
    u32 Height = Texture->Size.Height;

    v2i Dim = Buffer->Size;

    u32 *Pixels = (u32 *)Buffer->Pixels + ((u32)Origin.Y * Dim.Width) + (u32)Origin.X ;
    u32 *TexturePixels = (u32*)Texture->Pixels;

    for (int Y = 0; Y < Height; ++Y) 
    {
        for (int X = 0; X < Width; ++X)
        {
            if ((*(TexturePixels + X) >> 24) > 128) 
            {
                *(Pixels + X) = *(TexturePixels + X);
            }
        }
        Pixels += Dim.Width;
        TexturePixels += Width;
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

    // TODO: do this `PixelPerMeter` adjustment later.
    real32 Speed = 10 * PixelsPerMeter; // m/s * pixelsPerMeter = pixels per meter / s
    v2 Acceleration = MovementDirection * Speed;

    // TODO: proper friction to "decelerate"
    Acceleration += -0.9f * GameState->Velocity;

    v2 newVelocity = Acceleration * GameState->DeltaTime + GameState->Velocity;

    v2 NewPlayerP = 0.5f * Acceleration * Square(GameState->DeltaTime) + newVelocity * GameState->DeltaTime + GameState->PlayerPosition;
    
    GameState->PlayerPosition = NewPlayerP;
    GameState->Velocity = newVelocity;

    u32 BlueColor = 0xFFFF0000;
    v2 BluePos = V2(100, 100);
    v2i BlueSize = V2i(128, 128);
    DrawRectangle(Buffer, BluePos, BlueSize, BlueColor);

    u32 GreenColor = 0xFF00FF00;
    v2i GreenSize = V2i(128, 128);
    v2 GreenPos = V2(Buffer->Size.Width - GreenSize.Width - 100, 100);
    DrawRectangle(Buffer, GreenPos, GreenSize, GreenColor);

    // Read defs.h and print the result
    // debug_read_file_result Result = DebugReadEntireFile("../data/ship.png");
    debug_read_file_result Result = DebugReadEntireFile("../data/ship2.png");

    int Width, Height, Comp;
    char unsigned const *Contents = (char unsigned const *)Result.Content;
    char unsigned *Pixels = stbi_load_from_memory(Contents, Result.ContentSize, &Width, &Height, &Comp, STBI_rgb_alpha);

    loaded_texture Texture = {};
    v2 Size = {};
    Size.Width = Width;
    Size.Height = Height;
    Texture.Size = Size;
    Texture.Pixels = Pixels;

    DrawTexture(Buffer, GameState->PlayerPosition, &Texture);

    stbi_image_free(Pixels);

    DebugFreeFileMemory(Result.Content);
}
