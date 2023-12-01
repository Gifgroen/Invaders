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
    SystemR.Texture = &Assets->Ships[0];

    v2 OriginR = Center - V2((Center.width + SystemR.Texture->Size.width) * 0.5f, 0);
    SystemR.XAxis = (real32)SystemR.Texture->Size.width * V2(cosf(Angle), sinf(Angle));
    SystemR.YAxis = Perp(SystemR.XAxis);
    SystemR.Origin = OriginR - SystemR.XAxis * 0.5f - SystemR.YAxis * 0.5f;

    FillCoordinateSystem(Buffer, SystemR, 0xFFFFFF00);

    // G
    v2 OriginG = Center;
    coordinate_system SystemG = {};
    SystemG.Texture = &Assets->Ships[1];
    SystemG.XAxis = (real32)SystemG.Texture->Size.width * V2(cosf(Angle), sinf(Angle));
    SystemG.YAxis = Perp(SystemG.XAxis);
    SystemG.Origin = OriginG - SystemG.XAxis * 0.5f - SystemG.YAxis * 0.5f;

    FillCoordinateSystem(Buffer, SystemG, 0xFFFFFF00);

    // B
    coordinate_system SystemB = {};
    SystemB.Texture = &Assets->Ships[2];
    v2 OriginB = Center + V2((Center.width + SystemB.Texture->Size.width) * 0.5f, 0);
    SystemB.XAxis = (real32)SystemB.Texture->Size.width * V2(cosf(Angle), sinf(Angle));
    SystemB.YAxis = Perp(SystemB.XAxis);
    SystemB.Origin = OriginB - SystemB.XAxis * 0.5f - SystemB.YAxis * 0.5f;
    FillCoordinateSystem(Buffer, SystemB, 0xFFFFFF00);
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
    // TODO: Push render_element array.
    // TODO: Push onto Journal.
    render_group Group = {};

    // TODO: exract to some sort of PushRenderElement(Group, Element);
    render_element ClearElement = {};
    ClearElement = {};
    ClearElement.Type = element_type_Clear;
    ClearElement.Basis = {};

    Group.Elements[0] = ClearElement;
    ++Group.ElementIndex;

    // Remove the clear.
    Clear(Buffer, 0xFFFF00FF);

    DrawOutline(Buffer, V2(0.0f, 0.0f), Buffer->Size, 8, 0xff00ff00);

    game_state *GameState = (game_state*)GameMemory->PermanentStorage;

    // Coordinate System (Basis transform) test
    RotatingShips(Buffer, Buffer->Size, GameState);

    /* Simulate a player. */
    SimulatePlayerMovement(GameState, Input, Buffer->Size);

    /* Draw the simulated player */
    coordinate_system PlayerSystem = {};
    PlayerSystem.Origin = GameState->PlayerPosition;

    assets *Assets = GameState->Assets;
    // Get the asset.
    loaded_texture PlayerTexture = Assets->Ships[2];
    PlayerSystem.Texture = &PlayerTexture;
    // Determine position, rotation
    PlayerSystem.XAxis = V2(1.0f, 0.0f);
    PlayerSystem.YAxis = Perp(PlayerSystem.XAxis);
    // and scale
    PlayerSystem.XAxis *= PlayerTexture.Size.width;
    PlayerSystem.YAxis *= PlayerTexture.Size.height;
    // And draw it.
    FillCoordinateSystem(Buffer, PlayerSystem, 0xFFFFFF00);

    RenderToOutput(&Group, Buffer);

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
