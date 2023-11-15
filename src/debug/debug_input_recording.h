#ifndef DEBUG_INPUT_RECORDING_H
#define DEBUG_INPUT_RECORDING_H

// #include "game.h"
#include "../os_window.h"

enum record_action {
    Idle,
    Recording,
    Playing
};

struct debug_input_recording
{
    record_action Action;

    FILE *RecordHandle;
    FILE *PlaybackHandle;

    u64 TotalMemorySize;
};

// Record Input
internal_func void DebugBeginRecordInput(debug_input_recording *InputRecorder, game_memory *GameMemory);

internal_func void DebugRecordInput(debug_input_recording *InputRecorder, game_input *NewInput, game_memory *GameMemory);

internal_func void DebugEndRecordInput(debug_input_recording *InputRecorder);

// Playback Input
internal_func void DebugBeginPlaybackInput(debug_input_recording *InputRecorder, game_memory *GameMemory) ;

internal_func void DebugPlaybackInput(debug_input_recording *InputRecorder, game_input *NewInput, game_memory *GameMemory);

internal_func void DebugEndPlaybackInput(debug_input_recording *InputRecorder);

#if DEBUG // Debug Input handling
internal_func void DebugHandleKeyEvent(SDL_KeyboardEvent Event, window_state *WindowState, debug_input_recording *Recording, game_controller *KeyboardController);
#endif

#endif