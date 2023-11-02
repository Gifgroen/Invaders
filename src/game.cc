#include "game.h"

#include "assets.cc"
#include "debug_io.cc"
#include "math.cc"
#include "render.cc"

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
