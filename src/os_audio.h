#ifndef OS_AUDIO_H
#define OS_AUDIO_H

#include "game.h"

struct sdl_sound_output
{
    int SamplesPerSecond;
    int ToneHz;
    s16 ToneVolume;
    u32 RunningSampleIndex;
    int WavePeriod;
    int BytesPerSample;
    int SecondaryBufferSize;

    int SafetyBytes;
};

struct sdl_audio_ring_buffer
{
    int Size;
    int WriteCursor;
    int PlayCursor;
    void *Data;
};

struct sdl_audio_buffer_index 
{
    int ByteToLock;
    int TargetCursor;
    int BytesToWrite;
};

// SDL Audio
internal_func void SDLAudioCallback(void *UserData, u8 *AudioData, int Length);

internal_func void InitAudio(s32 SamplesPerSecond, s32 BufferSize);

internal_func sdl_audio_buffer_index  PositionAudioBuffer(sdl_sound_output *SoundOutput, int GameUpdateHz);

internal_func void FillSoundBuffer(sdl_sound_output *SoundOutput, int ByteToLock, int BytesToWrite, game_sound_output_buffer *SoundBuffer);

#endif // OS_AUDIO_H