#include "game.h"

#include "assets.cc"
#include "math.cc"
#include "memory.cc"
#include "player_movement.cc"
#include "render.cc"

void GameInit(game_memory *GameMemory, offscreen_buffer *Buffer)
{
    game_state *GameState = (game_state*)GameMemory->PermanentStorage;

    u8 *Base = (u8 *)GameMemory->PermanentStorage + sizeof(game_state);
    memory_size TotalSize = GameMemory->PermanentStorageSize - sizeof(game_state);
    InitialiseJournal(&GameState->Journal, Base, TotalSize);

    assets *Assets = PushStruct(&GameState->Journal, assets);
    Assets->BasePath = GameMemory->BasePath;
    GameState->Assets = Assets;

    AllocateAssets(GameMemory, Assets);

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

    render_group *Group = PushStruct(&GameState->Journal, render_group);
    Group->ElementsSpaceSize = Megabytes(8);
    Group->ElementsBase = (u8 *)PushSize(&GameState->Journal, Group->ElementsSpaceSize);

    v2 ScreenOrigin = V2(0.0f, 0.0f);
    PushClearElement(Group, ScreenOrigin, Buffer->Size, 0xFFFFFF00);
    PushOutlineElement(Group, ScreenOrigin, Buffer->Size, 8, 0xff00ff00);

    /* Simulate a player. */
    SimulatePlayerMovement(GameState, Input, Buffer->Size);
    /* Draw the simulated player */
    loaded_texture PlayerTexture = Assets->Ships[2];
    PushTextureElement(Group, &PlayerTexture, V2(0.0f, 0.0f), GameState->PlayerPosition);

    v2 ScreenCenter = V2((real32)Buffer->Size.width * 0.5f, (real32)Buffer->Size.height * 0.5f);
    real32 Rotation = GameState->ElapsedTime;

    PushRectElement(Group, V2(0.5f, 0.5f), ScreenCenter, V2i(256, 256), 0xff0000ff, -Rotation);

    /* Draw some sample Textures in CenterY of screen on a nice line. */
    // Left Red Ship
    loaded_texture *RTexture = &Assets->Ships[0];
    v2 OriginR = ScreenCenter - V2((ScreenCenter.width + (real32)RTexture->Size.width) * 0.5f, 0);
    PushTextureElement(Group, RTexture, V2(0.5f, 0.5f), OriginR,  Rotation);
    // Middle Green Ship
    PushTextureElement(Group, &Assets->Ships[1], V2(0.5f, 0.5f), ScreenCenter, Rotation);
    // Right Blue Ship
    loaded_texture *BTexture = &Assets->Ships[2];
    v2 OriginB = ScreenCenter + V2((ScreenCenter.width + (real32)BTexture->Size.width) * 0.5f, 0);
    PushTextureElement(Group, BTexture, V2(0.5f, 0.5f), OriginB, Rotation);

    // TODO: the Draw call for Outline do not support rotation yet
    // PushOutlineElement(Group, V2(100.0f, 100.0), V2i(50, 50), 4, 0xff0000ff, Rotation);

    RenderToOutput(Group, Buffer);

    // TODO: release the render_group memory properly
    GameState->Journal.Used -= Group->ElementsSpaceSize;
    Group->ElementsSpaceUsed = 0;
    GameState->Journal.Used -= sizeof(*Group);
    Group = NULL;

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
