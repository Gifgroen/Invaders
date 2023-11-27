#include "game.h"

#include "assets.cc"
// TODO: get rid of Debug IO and provide a struct with func pointers that offer i/o features.
#include "debug_io.cc"
#include "math.cc"
#include "memory.cc"
#include "render.cc"

#include <iostream>

global_variable coordinate_system *NewSystem;

void GameInit(game_memory *GameMemory, offscreen_buffer *Buffer)
{
    game_state *GameState = (game_state*)GameMemory->PermanentStorage;

    u8 *Base = (u8 *)GameMemory->PermanentStorage + sizeof(game_state);
    memory_size TotalSize = GameMemory->PermanentStorageSize - sizeof(game_state);
    InitialiseJournal(&GameState->Journal, Base, TotalSize);

    std::cout << "Loading assets from guessed relative path; should use assetPath = " << GameMemory->AssetPath << std::endl; 

    memory_size PathLen = strlen(GameMemory->AssetPath);

    char const *Path1 = "ship.png";
    memory_size Len1 = PathLen + strlen(Path1) + 1;
    char Ship1Path[Len1];
    snprintf(Ship1Path, Len1 + 11, "%s../../data/%s", GameMemory->AssetPath, Path1);

    loaded_texture Texture1 = LoadTexture(Ship1Path);
    GameState->Ships[0] = Texture1;

    char const *Path2 = "ship2.png";
    memory_size Len2 = PathLen + strlen(Path2) + 1;
    char Ship2Path[Len2];
    snprintf(Ship2Path, Len2 + 11, "%s../../data/%s", GameMemory->AssetPath, Path2);
    loaded_texture Texture2 = LoadTexture(Ship2Path);
    GameState->Ships[1] = Texture2;

    char const *Path3 = "ship3.png";
    memory_size Len3 = PathLen + strlen(Path3) + 1;
    char Ship3Path[Len3];
    snprintf(Ship3Path, Len3 + 11, "%s../../data/%s", GameMemory->AssetPath, Path3);
    loaded_texture Texture3 = LoadTexture(Ship3Path);
    GameState->Ships[2] = Texture3;   

    // Player
    v2 ShipSize = GameState->Ships[2].Size;
    GameState->PlayerSize = V2i(ShipSize.width, ShipSize.height);
    GameState->PlayerPosition = V2(0, 0);

    GameState->ToneHz = 256;

    GameState->Running = true;

    // Memory test
    NewSystem = PushStruct(&GameState->Journal, coordinate_system);
    NewSystem->XAxis = V2(1.0f, 0.0f);
    NewSystem->YAxis = Perp(NewSystem->XAxis);
    loaded_texture *Texture = &GameState->Ships[0];
    NewSystem->Texture = Texture;

    v2i ScreenSize = Buffer->Size;
    NewSystem->Origin = V2(
        ScreenSize.width - Texture->Size.width - 50.0f, 
        ScreenSize.height - Texture->Size.height - 50.0f 
    );

    NewSystem->XAxis *= Texture->Size.width;
    NewSystem->YAxis *= Texture->Size.height;
}

void GameUpdateAndRender(game_memory *GameMemory, offscreen_buffer *Buffer, game_input *Input)
{
    game_state *GameState = (game_state*)GameMemory->PermanentStorage;

    // GameState->ToneHz = 128;

    u32 Color = 0xFFFF00FF;
    Clear(Buffer, Color);

    real32 Angle = GameState->ElapsedTime;

    v2 Center = V2((real32)Buffer->Size.width * 0.5f, (real32)Buffer->Size.height * 0.5f);
    v2i Size = V2i(200, 200);

    // R
    v2 OriginR = Center - V2((Center.width + Size.width) / 2, 0);

    coordinate_system SystemR = {};
    SystemR.XAxis = (real32)Size.width * V2(cosf(Angle), sinf(Angle));
    SystemR.YAxis = Perp(SystemR.XAxis);
    SystemR.Origin = OriginR - SystemR.XAxis * 0.5f - SystemR.YAxis * 0.5f;
    SystemR.Texture = &GameState->Ships[1];
    FillCoordinateSystem(Buffer, SystemR, 0xFFFFFF00);

    // G
    v2 HalfSize = V2((real32)Size.x * 0.5, (real32)Size.height * 0.5);
    v2 OriginG = Center;

    coordinate_system SystemG = {};
    SystemG.XAxis = (real32)Size.width * V2(cosf(Angle), sinf(Angle));
    SystemG.YAxis = Perp(SystemG.XAxis);
    SystemG.Origin = OriginG - SystemG.XAxis * 0.5f - SystemG.YAxis * 0.5f;
    SystemG.Texture = &GameState->Ships[0];
    FillCoordinateSystem(Buffer, SystemG, 0xFFFFFF00);

    // B
    v2 OriginB = Center + V2((Center.width + Size.width) * 0.5, 0);
    coordinate_system SystemB = {};
    SystemB.XAxis = (real32)Size.width * V2(cosf(Angle), sinf(Angle));
    SystemB.YAxis = Perp(SystemB.XAxis);
    SystemB.Origin = OriginB - SystemB.XAxis * 0.5f - SystemB.YAxis * 0.5f;
    SystemB.Texture = &GameState->Ships[2];
    FillCoordinateSystem(Buffer, SystemB, 0xFFFFFF00);

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

    // TODO: Properly wrap around when leavind the screen on the sides?
    v2 NewPlayerP = 0.5f * Acceleration * Square(GameState->DeltaTime) + NewVelocity * GameState->DeltaTime + GameState->PlayerPosition;
    
    if (NewPlayerP.x <= 0)
    {
        NewPlayerP.x = 0;
        NewVelocity.x = 0;
    }
    if (NewPlayerP.x + GameState->PlayerSize.width >= Buffer->Size.width)
    {
        NewPlayerP.x = Buffer->Size.width - GameState->PlayerSize.width;
        NewVelocity.x = 0;
    }
    
    if (NewPlayerP.y <= 0)
    {
        NewPlayerP.y = 0;
        NewVelocity.y = 0;
    }
    if (NewPlayerP.y + GameState->PlayerSize.height >= Buffer->Size.height)
    {
        NewPlayerP.y = Buffer->Size.height - GameState->PlayerSize.height;
        NewVelocity.y = 0;
    }

    GameState->PlayerPosition = NewPlayerP;
    GameState->Velocity = NewVelocity;

    /** Draw a (player) Texture. */
    loaded_texture PlayerTexture = GameState->Ships[2];

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

    if (NewSystem) 
    {
        FillCoordinateSystem(Buffer, *NewSystem, 0xFF0000FF);
    }

    /** Playing with vectors: drawing a rotating texture. */
    coordinate_system System = {};
    System.Origin = V2((real32)Buffer->Size.width * 0.5f, (real32)Buffer->Size.height * 0.5f);
    System.XAxis = 200.0f * V2(cosf(Angle), sinf(Angle));
    System.YAxis = Perp(System.XAxis);

    System.Texture = &GameState->Ships[1];

    FillCoordinateSystem(Buffer, System, 0xFFFFFF00);

    /** Draw basis markers like Origin, X and Y axes, center. */
    DrawCoordinateSystem(Buffer, System);
    v2 SystemCenterPoint = V2(0.5f, 0.5f);
    DrawPointInCoordinateSystem(Buffer, System, SystemCenterPoint, 0xFF00FFFF);

    /** Draw a Rectangle aligned with the X and Y Axis */
    u32 GreenColor = 0xFF00FF00;
    v2i GreenSize = V2i(128, 128);
    v2 GreenPos = V2(Buffer->Size.width - GreenSize.width - 100, 100);
    DrawRectangle(Buffer, GreenPos, GreenSize, GreenColor);

    /** Draw a Rectangular outline given a Position and Thickness*/
    v2 OutlineOrigin = V2(100.0f, 100.0f);
    v2i OutlineSize = V2i(256, 256);
    u16 Thickness = 4;
    u32 OutlineColor = 0xFFFFFFFF;
    DrawOutline(Buffer, OutlineOrigin, OutlineSize, Thickness, OutlineColor);

    GameState->ElapsedTime += GameState->DeltaTime;
}

void GameOutputSound(game_sound_output_buffer *SoundBuffer, game_state *GameState)
{
    local_persist real32 tSine;
    s16 ToneVolume = 3000;

    int ToneHz = GameState->ToneHz > 0 ? GameState->ToneHz : 256;
    int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz;

    s16 *SampleOut = SoundBuffer->Samples;
    for(int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex)
    {
        real32 SineValue = sinf(tSine);
        s16 SampleValue = (s16)(SineValue * ToneVolume);
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;

        tSine += 2.0f * Pi32 * 1.0f / (real32)WavePeriod;
        if (tSine > 2.0f * Pi32) 
        {
            tSine -= 2.0f * Pi32;
        }
    }
}

void GameGetSoundSamples(game_memory *GameMemory, game_sound_output_buffer *SoundBuffer) 
{
    game_state *GameState = (game_state *)GameMemory->PermanentStorage;
    GameOutputSound(SoundBuffer, GameState);
}
