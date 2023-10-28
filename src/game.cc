#include "game.h"

#include "math.cc"
#include "debug_io.cc"

#include <iostream>

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
        u32 *Pixel = (u32*)Buffer->Pixels + (Y * Size.Width);
        for (u32 X = 0; X < Size.Width; ++X)
        {
            *Pixel++ = Color;
        }
    }
}

internal_func void DrawRectangle(offscreen_buffer *Buffer, v2 Origin, v2i Size, u32 Color) 
{
    // TODO: properly round Origin.X and Origin.Y.
    u32 Row = (u32)Origin.Y * Buffer->Size.Width;
    u32 Column = (u32)Origin.X;

    u32 *Pixel = (u32 *)Buffer->Pixels + Row + Column;

    for (s32 Y = 0; Y < Size.Height; ++Y)
    {
        for (s32 X = 0; X < Size.Width; ++X)
        {
            *Pixel++ = Color;
        }
        Pixel += Buffer->Size.Width - Size.Width;
    }
}

internal_func u32 AlphaBlend(u32 Texel, u32 Pixel)
{
    real32 SA = (real32)((Texel >> 24) & 0xFF);
    real32 RSA = SA / 255.0f;
    real32 SR = (real32)((Texel >> 16) & 0xFF);
    real32 SG = (real32)((Texel >> 8) & 0xFF);
    real32 SB = (real32)((Texel >> 0) & 0xFF);

    real32 DA = (real32)((Pixel >> 24) & 0xFF);
    real32 DR = (real32)((Pixel >> 16) & 0xFF);
    real32 DG = (real32)((Pixel >> 8) & 0xFF);
    real32 DB = (real32)((Pixel >> 0) & 0xFF);

    real32 InvRsa = (1.0f - RSA);
    real32 A = SA * RSA + DA * InvRsa;
    real32 R = SR * RSA + DR * InvRsa;
    real32 G = SG * RSA + DG * InvRsa;
    real32 B = SB * RSA + DB * InvRsa;

    return ((u32)(A + 0.5f) << 24)
        | ((u32)(R + 0.5f) << 16)
        | ((u32)(G + 0.5f) << 8)
        | ((u32)(B + 0.5f) << 0);
}

internal_func void DrawTexture(offscreen_buffer *Buffer, v2 Origin, loaded_texture *Texture)
{
    // Assert(Origin.x < Origin.x + Texture->Size.Width);
    // Assert(Origin.y < Origin.y + Texture->Size.Height);

    u32 TextureWidth = Texture->Size.Width;
    u32 TextureHeight = Texture->Size.Height;

    v2i BufferSize = Buffer->Size;

    u32 *Pixels = (u32 *)Buffer->Pixels + ((u32)Origin.Y * BufferSize.Width) + (u32)Origin.X;
    u32 *TexturePixel = (u32*)Texture->Pixels;

    for (int Y = 0; Y < TextureHeight; ++Y) 
    {
        for (int X = 0; X < TextureWidth; ++X)
        {
            *Pixels = AlphaBlend(*TexturePixel, *Pixels);

            Pixels++;
            TexturePixel++;
        }
        Pixels += BufferSize.Width - TextureWidth;
    }
}

struct coordinate_system 
{
    v2 Origin;
    v2 XAxis;
    v2 YAxis;
};

internal_func void DrawCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System, u32 Color)
{
    v2i AxisSize = V2i(8, 8);
    // Origin
    DrawRectangle(Buffer, System.Origin, AxisSize, Color);

    // X
    DrawRectangle(Buffer, System.Origin + 200 * System.XAxis, AxisSize, 0xFF00FF00);
    // Y
    DrawRectangle(Buffer, System.Origin + 200 * System.YAxis, AxisSize, 0xFF00FFFF);
}

internal_func void DrawPointInCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System, v2 Point, u32 Color)
{
    DrawRectangle(Buffer, System.Origin + Point.X * System.XAxis + Point.Y * System.YAxis, V2i(4, 4), Color);
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

    v2 ScreenCenter = V2((real32)Buffer->Size.Width * 0.5f, (real32)Buffer->Size.Height * 0.5f);

    /**
     * Playing with vectors
     */
    coordinate_system System = {};
    System.Origin = ScreenCenter;

    GameState->ElapsedTime += GameState->DeltaTime;

    real32 Angle = GameState->ElapsedTime;
    System.XAxis = V2(cosf(Angle), sinf(Angle));
    System.YAxis = V2(-System.XAxis.Y, System.XAxis.X);

    DrawCoordinateSystem(Buffer, System, 0xFFFF0000);

    v2 Point = V2(200.f, 200.f);
    DrawPointInCoordinateSystem(Buffer, System, Point, 0xFFFFFFFF);

    /**
     * Draw a Rectangle aligned with the X and Y Axis
     */
    u32 GreenColor = 0xFF00FF00;
    v2i GreenSize = V2i(128, 128);
    v2 GreenPos = V2(Buffer->Size.Width - GreenSize.Width - 100, 100);
    DrawRectangle(Buffer, GreenPos, GreenSize, GreenColor);

    // Read defs.h and print the result
    // debug_read_file_result Result = DebugReadEntireFile("../data/ship.png");
    // debug_read_file_result Result = DebugReadEntireFile("../data/ship2.png");
    debug_read_file_result Result = DebugReadEntireFile("../data/ship3.png");

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
