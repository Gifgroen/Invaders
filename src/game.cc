#include "game.h"

#include "assets.cc"
#include "math.cc"
#include "debug_io.cc"

#include <iostream>

void GameInit(game_memory *GameMemory, offscreen_buffer *Buffer)
{
    game_state *GameState = (game_state*)GameMemory->TransientStorage;

    // Player
    GameState->PlayerSize = V2i(500, 500);
    // v2i BufferSize = Buffer->Size;
    GameState->PlayerPosition = V2(
        0,
        0
        // (real32)BufferSize.width - GameState->PlayerSize.width) / 8, // X
        // (real32)BufferSize.height - GameState->PlayerSize.height - 16 // Y
    );

    GameState->Running = true;
    std::cout << "GameInit: running = " << GameState->Running << std::endl;
}

internal_func void Clear(offscreen_buffer *Buffer, u32 Color) 
{
    v2i Size = Buffer->Size;
    for (u32 Y = 0; Y < Size.height; ++Y)
    {
        u32 *Pixel = (u32*)Buffer->Pixels + (Y * Size.width);
        for (u32 X = 0; X < Size.width; ++X)
        {
            *Pixel++ = Color;
        }
    }
}

internal_func void DrawRectangle(offscreen_buffer *Buffer, v2 Origin, v2i Size, u32 Color) 
{
    // TODO: properly round Origin.X and Origin.Y.
    u32 Row = (u32)Origin.y * Buffer->Size.width;
    u32 Column = (u32)Origin.x;

    u32 *Pixel = (u32 *)Buffer->Pixels + Row + Column;

    for (u32 Y = 0; Y < Size.height; ++Y)
    {
        for (u32 X = 0; X < Size.width; ++X)
        {
            *Pixel++ = Color;
        }
        Pixel += Buffer->Size.width - Size.width;
    }
}

internal_func u32 AlphaBlend(u32 Texel, u32 Pixel)
{
    real32 SA = (real32)((Texel >> 24) & 0xFF);
    real32 SR = (real32)((Texel >> 16) & 0xFF);
    real32 SG = (real32)((Texel >> 8) & 0xFF);
    real32 SB = (real32)((Texel >> 0) & 0xFF);

    real32 DA = (real32)((Pixel >> 24) & 0xFF);
    real32 DR = (real32)((Pixel >> 16) & 0xFF);
    real32 DG = (real32)((Pixel >> 8) & 0xFF);
    real32 DB = (real32)((Pixel >> 0) & 0xFF);

    real32 RSA = SA / 255.0f;
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
    Assert(Origin.x >= 0 && Origin.x < Origin.x + Texture->Size.width);
    Assert(Origin.y >= 0 && Origin.y < Origin.y + Texture->Size.height);

    u32 TextureWidth = Texture->Size.width;
    u32 TextureHeight = Texture->Size.height;

    v2i BufferSize = Buffer->Size;

    u32 *Pixels = (u32 *)Buffer->Pixels + ((u32)Origin.y * BufferSize.width) + (u32)Origin.x;
    u32 *TexturePixel = (u32*)Texture->Pixels;

    for (int Y = 0; Y < TextureHeight; ++Y) 
    {
        for (int X = 0; X < TextureWidth; ++X)
        {
            *Pixels = AlphaBlend(*TexturePixel, *Pixels);

            Pixels++;
            TexturePixel++;
        }
        Pixels += BufferSize.width - TextureWidth;
    }
}

struct coordinate_system 
{
    v2 Origin;
    v2 XAxis;
    v2 YAxis;

    loaded_texture *Texture;
};

internal_func void FillCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System, u32 Color)
{
    v2 Points[4] = {
        System.Origin,
        System.Origin + System.XAxis,
        System.Origin + System.XAxis + System.YAxis,
        System.Origin + System.YAxis,
    };

    real32 xMin = Buffer->Size.width;
    real32 xMax = 0;
    real32 yMin = Buffer->Size.height;
    real32 yMax = 0;
    for (int i = 0; i < ArrayCount(Points); ++i )
    {
        v2 Point = Points[i];
        if (Point.x < xMin)
        {
            xMin = Point.x;
        }
        if (Point.x > xMax)
        {
            xMax = Point.x;
        }
        if (Point.y < yMin)
        {
            yMin = Point.y;
        }
        if (Point.y > yMax)
        {
            yMax = Point.y;
        }
    }

    real32 InvXAxisLengthSq = 1.0f / LengthSq(System.XAxis);
    real32 InvYAxisLengthSq = 1.0f / LengthSq(System.YAxis);

    for (int Y = yMin; Y < yMax; ++Y)
    {
        for(int X = xMin; X < xMax; ++X)
        {
            u32 *Pixel = (u32*)Buffer->Pixels + (u32)Y * Buffer->Size.width + (u32)X;
#if 1
            v2 Point = V2(X, Y);
            v2 d = Point - System.Origin;
            int Edge0 = Dot(d, -Perp(System.XAxis));
            int Edge1 = Dot(d - System.XAxis, -Perp(System.YAxis));
            int Edge2 = Dot(d - System.XAxis - System.YAxis, Perp(System.XAxis));
            int Edge3 = Dot(d - System.YAxis, Perp(System.YAxis));

            if (Edge0 < 0 && Edge1 < 0 && Edge2 < 0 && Edge3 < 0) 
            {
                real32 U = InvXAxisLengthSq * Dot(d, System.XAxis);
                real32 V = InvYAxisLengthSq * Dot(d, System.YAxis);

                Assert( U >= 0.0f && U <= 1.0f );
                Assert( V >= 0.0f && V <= 1.0f );

                u32 X = (u32)(U * (real32)(System.Texture->Size.width - 3) + 0.5f);
                u32 Y = (u32)(V * (real32)(System.Texture->Size.height - 3) + 0.5f);

                Assert(X >= 0 && X < System.Texture->Size.width);
                Assert(Y >= 0 && Y < System.Texture->Size.height);

                u32 *TexturePixel = (u32*)System.Texture->Pixels + (u32)(Y * System.Texture->Size.width) + X;
                *Pixel = AlphaBlend(*TexturePixel, *Pixel);
            }
#else 
            *Pixel = Color;
#endif
        }
    }
}

internal_func void DrawCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System)
{
    v2i AxisSize = V2i(8, 8);
    // Origin
    DrawRectangle(Buffer, System.Origin, AxisSize, 0x00000000);

    // X Axis
    DrawRectangle(Buffer, System.Origin + System.XAxis, AxisSize, 0xFF0000FF);
    // Y Axis
    DrawRectangle(Buffer, System.Origin + System.YAxis, AxisSize, 0xFF0000FF);
}

internal_func void DrawPointInCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System, v2 Point, u32 Color)
{
    DrawRectangle(Buffer, System.Origin + Point.x * System.XAxis + Point.y * System.YAxis, V2i(16, 16), Color);
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
        MovementDirection.x = -1.f;
    }
    if (Keyboard.MoveRight.IsDown || Controller.MoveRight.IsDown)
    {
        MovementDirection.x = 1.f;
    }
    if (Keyboard.MoveUp.IsDown || Controller.MoveUp.IsDown)
    {
        MovementDirection.y = -1.f;
    }
    if (Keyboard.MoveDown.IsDown || Controller.MoveDown.IsDown)
    {
        MovementDirection.y = 1.f;
    }

    if (MovementDirection.x != 0.f && MovementDirection.y != 0.f)
    {
        MovementDirection *= 0.707106f;
    }

    int PixelsPerMeter = 64;

    // TODO: do this `PixelPerMeter` adjustment later.
    real32 Speed = 10 * PixelsPerMeter; // m/s * pixelsPerMeter = pixels per meter / s
    v2 Acceleration = MovementDirection * Speed;

    // TODO: proper friction to "decelerate"
    Acceleration += -0.9f * GameState->Velocity;

    v2 NewVelocity = Acceleration * GameState->DeltaTime + GameState->Velocity;

    v2 NewPlayerP = 0.5f * Acceleration * Square(GameState->DeltaTime) + NewVelocity * GameState->DeltaTime + GameState->PlayerPosition;
    
    GameState->PlayerPosition = NewPlayerP;
    GameState->Velocity = NewVelocity;

    /** Draw a (player) Texture. */
    char const *PlayerPath = "../data/ship3.png";
    loaded_texture PlayerTexture = LoadTexture(PlayerPath);
    
#if 1
    coordinate_system PlayerSystem = {};
    PlayerSystem.Origin = GameState->PlayerPosition;
    
    PlayerSystem.Texture = &PlayerTexture;
    PlayerSystem.XAxis = V2(1.0f, 0.0f);
    PlayerSystem.YAxis = Perp(PlayerSystem.XAxis);

    // Scale both axes of the coordinate_system based on texture size(s).
    PlayerSystem.XAxis *= PlayerTexture.Size.width;
    PlayerSystem.YAxis *= PlayerTexture.Size.height;

    FillCoordinateSystem(Buffer, PlayerSystem, 0xFFFFFF00);
#else
    DrawTexture(Buffer, GameState->PlayerPosition, &PlayerTexture);
#endif

    /** Playing with vectors: drawing a rotating texture. */
    coordinate_system System = {};

    v2 ScreenCenter = V2((real32)Buffer->Size.width * 0.5f, (real32)Buffer->Size.height * 0.5f);
    System.Origin = ScreenCenter;

    GameState->ElapsedTime += GameState->DeltaTime;

    real32 Angle = GameState->ElapsedTime;
    System.XAxis = 200.0f * V2(cosf(Angle), sinf(Angle));
    System.YAxis = Perp(System.XAxis);

    char const *ShipPath = "../data/ship2.png";
    loaded_texture ShipTexture = LoadTexture(ShipPath);
    System.Texture = &ShipTexture;

    FillCoordinateSystem(Buffer, System, 0xFFFFFF00);

    DrawCoordinateSystem(Buffer, System);
    v2 SystemCenterPoint = V2(0.5f, 0.5f);
    DrawPointInCoordinateSystem(Buffer, System, SystemCenterPoint, 0xFF00FFFF);

    /**
     * Draw a Rectangle aligned with the X and Y Axis
     */
    u32 GreenColor = 0xFF00FF00;
    v2i GreenSize = V2i(128, 128);
    v2 GreenPos = V2(Buffer->Size.width - GreenSize.width - 100, 100);
    DrawRectangle(Buffer, GreenPos, GreenSize, GreenColor);
}
