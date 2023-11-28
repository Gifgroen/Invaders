#include "game.h"

#include "assets.cc"
// TODO: get rid of Debug IO and provide a struct with func pointers that offer i/o features.
#include "debug_io.cc"
#include "math.cc"
#include "memory.cc"
#include "player_movement.cc"
#include "render.cc"

/* Sample test */
internal_func void RotatingShips(offscreen_buffer *Buffer, v2i ScreenSize, game_state *GameState)
{
    v2 Center = V2((real32)ScreenSize.width * 0.5f, (real32)ScreenSize.height * 0.5f);
    v2i Size = V2i(200, 200);

    real32 Angle = GameState->ElapsedTime;

    // R
    v2 OriginR = Center - V2((Center.width + Size.width) / 2, 0);
    coordinate_system SystemR = {};
    SystemR.XAxis = (real32)Size.width * V2(cosf(Angle), sinf(Angle));
    SystemR.YAxis = Perp(SystemR.XAxis);
    SystemR.Origin = OriginR - SystemR.XAxis * 0.5f - SystemR.YAxis * 0.5f;
    SystemR.Texture = &GameState->Ships[0];
    FillCoordinateSystem(Buffer, SystemR, 0xFFFFFF00);

    // G
    v2 OriginG = Center;
    coordinate_system SystemG = {};
    SystemG.XAxis = (real32)Size.width * V2(cosf(Angle), sinf(Angle));
    SystemG.YAxis = Perp(SystemG.XAxis);
    SystemG.Origin = OriginG - SystemG.XAxis * 0.5f - SystemG.YAxis * 0.5f;
    SystemG.Texture = &GameState->Ships[1];
    FillCoordinateSystem(Buffer, SystemG, 0xFFFFFF00);

    // B
    v2 OriginB = Center + V2((Center.width + Size.width) * 0.5, 0);
    coordinate_system SystemB = {};
    SystemB.XAxis = (real32)Size.width * V2(cosf(Angle), sinf(Angle));
    SystemB.YAxis = Perp(SystemB.XAxis);
    SystemB.Origin = OriginB - SystemB.XAxis * 0.5f - SystemB.YAxis * 0.5f;
    SystemB.Texture = &GameState->Ships[2];
    FillCoordinateSystem(Buffer, SystemB, 0xFFFFFF00);
}

void GameInit(game_memory *GameMemory, offscreen_buffer *Buffer)
{
    game_state *GameState = (game_state*)GameMemory->PermanentStorage;

    u8 *Base = (u8 *)GameMemory->PermanentStorage + sizeof(game_state);
    memory_size TotalSize = GameMemory->PermanentStorageSize - sizeof(game_state);
    InitialiseJournal(&GameState->Journal, Base, TotalSize);

    memory_size PathLen = strlen(GameMemory->AssetPath);

    char const *Path1 = "playerShip1_red.png";
    memory_size Len1 = PathLen + strlen(Path1) + 1;
    char Ship1Path[Len1];
    snprintf(Ship1Path, Len1 + 11, "%s../../data/%s", GameMemory->AssetPath, Path1);
    loaded_texture Texture1 = LoadTexture(Ship1Path);
    GameState->Ships[0] = Texture1;

    char const *Path2 = "playerShip2_green.png";
    memory_size Len2 = PathLen + strlen(Path2) + 1;
    char Ship2Path[Len2];
    snprintf(Ship2Path, Len2 + 11, "%s../../data/%s", GameMemory->AssetPath, Path2);
    loaded_texture Texture2 = LoadTexture(Ship2Path);
    GameState->Ships[1] = Texture2;

    char const *Path3 = "playerShip3_blue.png";
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
}

void GameUpdateAndRender(game_memory *GameMemory, offscreen_buffer *Buffer, game_input *Input)
{
    Clear(Buffer, 0xFFFF00FF);

    game_state *GameState = (game_state*)GameMemory->PermanentStorage;

    // Coordinate System (Basis transform) test
    RotatingShips(Buffer, Buffer->Size, GameState);

    /* simulate a player. */
    SimulatePlayerMovement(GameState, Input, Buffer->Size);
    /* Draw the simulated player */
    coordinate_system PlayerSystem = {};
    PlayerSystem.Origin = GameState->PlayerPosition;

    loaded_texture PlayerTexture = GameState->Ships[2];
    PlayerSystem.Texture = &PlayerTexture;
    PlayerSystem.XAxis = PlayerTexture.Size.width * V2(1.0f, 0.0f);
    PlayerSystem.YAxis = Perp(PlayerSystem.XAxis);

    FillCoordinateSystem(Buffer, PlayerSystem, 0xFFFFFF00);

    // Update sim administration
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
