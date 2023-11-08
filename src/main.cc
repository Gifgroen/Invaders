#include <iostream>

#if defined(PLATFORM_WIN)
#include <intrin.h>
#elif defined(PLATFORM_MACOS)
#include <x86intrin.h>
#endif

#if defined(PLATFORM_WIN)
#include "SDL.h"
#elif defined(PLATFORM_MACOS)
#include <SDL2/SDL.h>
#else
// TODO: Assert and crash?
#endif

#if defined(PLATFORM_WIN)
// #include <memoryapi.h>
#elif defined(PLATFORM_MACOS)
#include <sys/types.h>
#include <sys/mman.h>
#else
// TODO: Assert and crash?
#endif

#include "defs.h"
#include "types.h"
#include "game.h"

#include "framerate.cc"
#include "gamelib.cc"
#include "math.cc"
#include "os_audio.cc"
#include "os_input.cc"
#include "os_window.cc"

#if DEBUG
#include "debug_sync_display.h"
#include "debug_sync_display.cc"

#include "debug_input_recording.h"
#include "debug_input_recording.cc"
#endif

internal_func int GetWindowRefreshRate(SDL_Window *Window)
{
    SDL_DisplayMode Mode;
    int DisplayIndex = SDL_GetWindowDisplayIndex(Window);

    int DefaultRefreshRate = 60;
    if (SDL_GetDesktopDisplayMode(DisplayIndex, &Mode) != 0)
    {
        return DefaultRefreshRate;
    }
    if (Mode.refresh_rate == 0)
    {
        return DefaultRefreshRate;
    }
    return Mode.refresh_rate;
}

int GameMain(int Argc, char *Args[])
{
#if DEBUG 
    std::cout << "[Debug] " << (Argc - 1) << " arguments passed." << std::endl;
#endif

    window_state WindowState = {};
    WindowState.Title = "Invaders must die!";
    WindowState.Size = V2i(1024, 768);
    if (InitWindow(&WindowState) == false)
    {
        std::cout << "[Error] Initialising the window failed!" << std::endl;
        return -1;
    }

    game_lib GameLib = {};
#if defined(PLATFORM_MACOS)
    GameLib.LibPath = "../build/macos/libgame.so";
#elif defined(PLATFORM_WIN)
    GameLib.LibPath = "../build/win/libgame.dll";
#endif
    if (LoadGameCode(&GameLib) != 0)
    {
        std::cout << "[Error] Loading GameCode failed!" << std::endl;
        return -1;
    }
    
    offscreen_buffer BackBuffer = {};
    UpdateOffscreenBuffer(&WindowState, &BackBuffer);

    game_memory GameMemory = {};
    GameMemory.TransientStorageSize = Megabytes(64);
    GameMemory.PermanentStorageSize = Gigabytes(2);

debug_input_recording InputRecorder = {};
#if DEBUG
    void *BaseAddress = (void *)Terabytes(2);
#else
    void *BaseAddress = (void *)(0);
#endif

    u64 TotalStorageSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
    InputRecorder.TotalMemorySize = TotalStorageSize;
#if defined(PLATFORM_WIN)
    GameMemory.PermanentStorage = VirtualAlloc(BaseAddress, TotalStorageSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
#elif defined(PLATFORM_MACOS)
    GameMemory.PermanentStorage = mmap(BaseAddress, TotalStorageSize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
#endif
    GameMemory.TransientStorage = (u8*)(GameMemory.PermanentStorage) + GameMemory.PermanentStorageSize;
    Assert(GameMemory.PermanentStorage);
    Assert(GameMemory.TransientStorage);

    int const GameUpdateHz = 30;
    real32 TargetSecondsPerFrame = 1.0f / (real64)GameUpdateHz;
    int DetectedFrameRate = GetWindowRefreshRate(WindowState.Window);
    if (DetectedFrameRate != GameUpdateHz)
    {
        std::cout << "Device capable refresh rate is " << DetectedFrameRate << " Hz, but Game runs in " << GameUpdateHz << " Hz\n";
    }

    game_state *GameState = (game_state *)GameMemory.TransientStorage;
    GameState->DeltaTime = 1.f / (real32)GameUpdateHz;

    sdl_sound_output SoundOutput = {};
    SoundOutput.SamplesPerSecond = 48000;
    SoundOutput.ToneHz = 256;
    SoundOutput.ToneVolume = 3000;
    SoundOutput.RunningSampleIndex = 0;
    SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond / SoundOutput.ToneHz;
    SoundOutput.BytesPerSample = sizeof(s16) * 2;
    SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;
    // TODO: compute variance in the frame time so we can choose what the lowest reasonable value is.
    SoundOutput.SafetyBytes = 0.25 * ((SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample) / GameUpdateHz);

    InitAudio(SoundOutput.SamplesPerSecond, SoundOutput.SecondaryBufferSize);
    s16 *Samples = (s16 *)calloc(SoundOutput.SamplesPerSecond, SoundOutput.BytesPerSample);
    bool SoundIsPlaying = false;

    // Setup Input
    game_input Input[2] = {};
    game_input *OldInput = &Input[0];
    game_input *NewInput = &Input[1];
    // Controllers
    OpenInputControllers();
    
    // Ticks per second for this CPU.
    u64 PerfCountFrequency = SDL_GetPerformanceFrequency();

    // Administration to enforce a framerate of GameUpdateHz
    u64 StartCycleCount = __rdtsc();
    u64 StartTime = SDL_GetPerformanceCounter();

    sdl_debug_time_marker DebugTimeMarkers[GameUpdateHz / 2] = {};
    u64 DebugLastPlayCursorIndex = 0;

    GameLib.GameInit(&GameMemory, &BackBuffer);

    while (GameState->Running)
    {
        game_controller *OldKeyboardController = GetKeyboardForIndex(OldInput, 0);
        game_controller *NewKeyboardController = GetKeyboardForIndex(NewInput, 0);
        *NewKeyboardController = {};
        for(u64 ButtonIndex = 0; ButtonIndex < ArrayCount(NewKeyboardController->Buttons); ++ButtonIndex)
        {
            game_button_state *NewButtons = &(NewKeyboardController->Buttons[ButtonIndex]);
            game_button_state *OldButtons = &(OldKeyboardController->Buttons[ButtonIndex]);
            NewButtons->IsDown = OldButtons->IsDown;
            NewButtons->HalfTransitionCount = OldButtons->HalfTransitionCount;
        }
        
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_WINDOWEVENT:
                {
                    ProcessWindowEvent(&e.window, &WindowState, &BackBuffer);
                } break;

                case SDL_QUIT:
                {
                    GameState->Running = false;
                } break;

                case SDL_KEYUP:
                case SDL_KEYDOWN:
                {
                    #if DEBUG
                    DebugHandleKeyEvent(e.key, &WindowState, &InputRecorder, NewKeyboardController);
                    #endif
                    ProcessKeyboardEvents(&e, GameState, NewKeyboardController);
                } break;
            }
        }
        HandleControllerEvents(OldInput, NewInput);

#if DEBUG
        if (InputRecorder.Action == record_action::Recording)
        {
            DebugRecordInput(&InputRecorder, NewInput, &GameMemory);
        }
        if (InputRecorder.Action == record_action::Playing)
        {
            DebugEndRecordInput(&InputRecorder);
            DebugPlaybackInput(&InputRecorder, NewInput, &GameMemory);
        }
#endif

        if (GameCodeChanged(&GameLib) > GameLib.LastWriteTime)
        {
            std::cout << "[Info] GameCode has changed, reloading!\n" << std::endl;
            LoadGameCode(&GameLib);
        }
        
        // TODO: simulate game
        GameLib.GameUpdateAndRender(&GameMemory, &BackBuffer, NewInput);

        // REGION: Write Audio to Ringbuffer

        SDL_LockAudio();
        
        sdl_audio_buffer_index AudioBufferIndex = PositionAudioBuffer(&SoundOutput, GameUpdateHz);

        game_sound_output_buffer SoundBuffer = {};
        SoundBuffer.SamplesPerSecond = SoundOutput.SamplesPerSecond;
        SoundBuffer.SampleCount = AudioBufferIndex.BytesToWrite / SoundOutput.BytesPerSample;
        SoundBuffer.Samples = Samples;

        GameLib.GameGetSoundSamples(&GameMemory, &SoundBuffer);

        SDL_UnlockAudio();

        FillSoundBuffer(&SoundOutput, AudioBufferIndex.ByteToLock, AudioBufferIndex.BytesToWrite, &SoundBuffer); 

#if DEBUG // SOUND SYNC DEBUG
        int UnwrappedWriteCursor = AudioRingBuffer.WriteCursor;
        if (UnwrappedWriteCursor < AudioRingBuffer.PlayCursor)
        {
            UnwrappedWriteCursor += SoundOutput.SecondaryBufferSize;
        }

        int AudioLatencyBytes = UnwrappedWriteCursor - AudioRingBuffer.PlayCursor;
        real32 AudioLatencySeconds = (((real32)AudioLatencyBytes / (real32)SoundOutput.BytesPerSample) / (real32)SoundOutput.SamplesPerSecond);
        std::cout << "BTL: " << AudioBufferIndex.ByteToLock << ", ";
        std::cout << "TC: " << AudioBufferIndex.TargetCursor << ", ";
        std::cout << "BTW: " << AudioBufferIndex.BytesToWrite << ", ";
        std::cout << "- PC: " << AudioRingBuffer.PlayCursor << ", ";
        std::cout << "WC:"  << AudioRingBuffer.WriteCursor << ", ";
        std::cout << "DELTA: " << AudioLatencyBytes << " (" << AudioLatencySeconds << "s)";
        std::cout << std::endl;
#endif
        if(!SoundIsPlaying)
        {
            SDL_PauseAudio(0);
            SoundIsPlaying = true;
        }

        // END REGION: Write Audio to Ringbuffer

        TryWaitForNextFrame(StartTime, TargetSecondsPerFrame);

        // Measure the Game time, ignore SDL Update time.
        u64 EndTime = SDL_GetPerformanceCounter();

#if DEBUG
        SDLDebugSyncDisplay(&BackBuffer, ArrayCount(DebugTimeMarkers), DebugTimeMarkers, &SoundOutput, TargetSecondsPerFrame);
#endif

        UpdateWindow(&WindowState, BackBuffer.Pixels);

#if DEBUG
        {
            sdl_debug_time_marker *marker = &DebugTimeMarkers[DebugLastPlayCursorIndex++];
            if (DebugLastPlayCursorIndex >= ArrayCount(DebugTimeMarkers))
            {
                DebugLastPlayCursorIndex = 0;
            }
            marker->PlayCursor = AudioRingBuffer.PlayCursor;
            marker->WriteCursor = AudioRingBuffer.WriteCursor;
        }
#endif
        game_input *Tmp = OldInput;
        OldInput = NewInput;
        NewInput = Tmp;
        *NewInput = {};

        /// 
        /// Profiling
        ///

        // Cycles
        u64 EndCycleCount = __rdtsc();
        u64 CycleCount = EndCycleCount - StartCycleCount;
        // Accumulate
        u64 ElapsedTime = EndTime - StartTime;

        /// Visualise
        // Time: Convert frametime in (ms/f)
        real64 MSPF = (1000.0f * (real64)ElapsedTime) / (real64)PerfCountFrequency;
        // FPS: compare CPU counter with ticks per seconds to Rate (f/s)
        real64 FPS = (real64)PerfCountFrequency / (real64)ElapsedTime;

        // Cycles devided by a million = Mega Cycles per frame (mc/f).
        real64 MCPF = ((real64)CycleCount / (1000.0f * 1000.0f));
        std::cout << "Time: " << MSPF << "ms/f, FPS: " << FPS << "f/s, Cycles: " << MCPF << "mc/f" << std::endl;
        StartCycleCount = EndCycleCount;
        StartTime = EndTime;
    }

    DestroyWindow(&WindowState);
    DestroyBackBuffer(&BackBuffer);
    return 0;
}


#if defined(PLATFORM_MACOS)
int main(int Argc, char *Argv[])
{
     return GameMain(Argc, Argv);
}
#elif defined(PLATFORM_WIN)
int wmain(int Argc, char *Argv[])
{
     return GameMain(Argc, Argv); 
}
#else 
// NO SUPPORT FOR UNKNOWN PLATFORM
#endif