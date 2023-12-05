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
    real32 Angle = GameState->ElapsedTime;

    assets *Assets = GameState->Assets;

    v2 Center = V2((real32)ScreenSize.width * 0.5f, (real32)ScreenSize.height * 0.5f);

    // R
    coordinate_system SystemR = {};
    loaded_texture *RTexture = &Assets->Ships[0];

    v2 OriginR = Center - V2((Center.width + RTexture->Size.width) * 0.5f, 0);
    SystemR.XAxis = (real32)RTexture->Size.width * V2(cosf(Angle), sinf(Angle));
    SystemR.YAxis = Perp(SystemR.XAxis);
    SystemR.Origin = OriginR - SystemR.XAxis * 0.5f - SystemR.YAxis * 0.5f;

    FillCoordinateSystem(Buffer, RTexture, 0xFFFFFF00, SystemR);

    // G
    v2 OriginG = Center;
    coordinate_system SystemG = {};
    loaded_texture *GTexture = &Assets->Ships[1];
    SystemG.XAxis = (real32)GTexture->Size.width * V2(cosf(Angle), sinf(Angle));
    SystemG.YAxis = Perp(SystemG.XAxis);
    SystemG.Origin = OriginG - SystemG.XAxis * 0.5f - SystemG.YAxis * 0.5f;

    FillCoordinateSystem(Buffer, GTexture, 0xFFFFFF00, SystemG);

    // B
    coordinate_system SystemB = {};
    loaded_texture *BTexture = &Assets->Ships[2];
    v2 OriginB = Center + V2((Center.width + BTexture->Size.width) * 0.5f, 0);
    SystemB.XAxis = (real32)BTexture->Size.width * V2(cosf(Angle), sinf(Angle));
    SystemB.YAxis = Perp(SystemB.XAxis);
    SystemB.Origin = OriginB - SystemB.XAxis * 0.5f - SystemB.YAxis * 0.5f;
    FillCoordinateSystem(Buffer, BTexture, 0xFFFFFF00, SystemB);
}

void GameInit(game_memory *GameMemory, offscreen_buffer *Buffer)
{
    game_state *GameState = (game_state*)GameMemory->PermanentStorage;

    u8 *Base = (u8 *)GameMemory->PermanentStorage + sizeof(game_state);
    memory_size TotalSize = GameMemory->PermanentStorageSize - sizeof(game_state);
    InitialiseJournal(&GameState->Journal, Base, TotalSize);

    assets *Assets = PushStruct(&GameState->Journal, assets);
    Assets->BasePath = GameMemory->BasePath;
    GameState->Assets = Assets;

    AllocateAssets(Assets);

    // Init Game
    v2 ShipSize = Assets->Ships[2].Size;
    GameState->PlayerSize = V2i(ShipSize.width, ShipSize.height);
    GameState->PlayerPosition = V2(0, 0);

    GameState->ToneHz = 256;
    GameState->Running = true;
}

void GameUpdateAndRender(game_memory *GameMemory, offscreen_buffer *Buffer, game_input *Input)
{
    game_state *GameState = (game_state*)GameMemory->PermanentStorage;

    assets *Assets = GameState->Assets;

    // TODO: Push render_element array to Journal?

    render_group *Group = PushStruct(&GameState->Journal, render_group);

    v2 ScreenOrigin = V2(0.0f, 0.0f);
    PushClearElement(Group, ScreenOrigin, Buffer->Size, 0xFFFFFF00);
    // PushOutlineElement(Group, ScreenOrigin, Buffer->Size, 8, 0xff00ff00);

    // // Coordinate System (Basis transform) test
    // RotatingShips(Buffer, Buffer->Size, GameState);

    // /* Simulate a player. */
    // SimulatePlayerMovement(GameState, Input, Buffer->Size);

    // /* Draw the simulated player */
    // coordinate_system PlayerSystem = {};
    // // Determine position, rotation
    // PlayerSystem.Origin = GameState->PlayerPosition;
    // PlayerSystem.XAxis = V2(1.0f, 0.0f);
    // PlayerSystem.YAxis = Perp(PlayerSystem.XAxis);
    // // and scale
    // loaded_texture PlayerTexture = Assets->Ships[2];
    // PlayerSystem.XAxis *= PlayerTexture.Size.width;
    // PlayerSystem.YAxis *= PlayerTexture.Size.height;
    // // And draw it.
    // FillCoordinateSystem(Buffer, &PlayerTexture, 0xFFFFFF00, PlayerSystem);

    RenderToOutput(Group, Buffer);

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
