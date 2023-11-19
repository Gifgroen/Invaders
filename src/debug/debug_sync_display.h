#ifndef DEBUG_SYNC_DISPLAY_H
#define DEBUG_SYNC_DISPLAY_H

#include "../defs.h"
#include "../types.h"

#include "../os_audio.h"

struct sdl_debug_time_marker 
{
    int PlayCursor;
    int WriteCursor;
};

internal_func void SDLDebugDrawVertical(offscreen_buffer *Buffer, int Value, int Top, int Bottom, u32 Color);

inline void SDLDrawSoundBufferMarker(
    offscreen_buffer *Buffer,
    sdl_sound_output *SoundOutput,
    real32 C, 
    int PadX, 
    int Top, 
    int Bottom,
    int Value, 
    u32 Color
);

internal_func void SDLDebugSyncDisplay(
    offscreen_buffer *Buffer, 
    int DebugTimeMarkerCount, 
    sdl_debug_time_marker *DebugTimeMarkers,
    sdl_sound_output *SoundOutput, 
    real64 TargetSecondsPerFrame
);

#endif // DEBUG_SYNC_DISPLAY_H