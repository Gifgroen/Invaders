#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include <x86intrin.h>

#include "defs.h"
#include "os_window.cc"
#include "os_input.cc"
#include "gamelib.cc"
#include "framerate.cc"

internal int GetWindowRefreshRate(SDL_Window *Window)
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

int main(int Argc, char *Args[])
{
#if DEBUG 
    printf("[Debug] %d arguments passed. ", Argc - 1);
#endif

    window_state WindowState = {};
    WindowState.Title = "Invaders must die!";
    WindowState.Size = V2(1024, 768);
    if (InitWindow(&WindowState) == false)
    {
        printf("[Error] Initialising the window failed!\n");
        return -1;
    }

    game_lib GameLib = {};
    GameLib.LibPath = "../build/libgame.so";
    if (LoadGameCode(&GameLib) != 0)
    {
        printf("[Error] Loading GameCode failed!\n");
        return -1;
    }
    
    offscreen_buffer BackBuffer = {};
    UpdateOffscreenBuffer(&WindowState, &BackBuffer);

    game_state GameState = {};
    GameState.Running = true;

    int const GameUpdateHz = 30;
    real32 TargetSecondsPerFrame = 1.0f / (real64)GameUpdateHz;
    int DetectedFrameRate = GetWindowRefreshRate(WindowState.Window);
    if (DetectedFrameRate != GameUpdateHz)
    {
        printf("Device capable refresh rate is %d Hz, but Game runs in %d Hz\n", DetectedFrameRate, GameUpdateHz);
    }
    // Ticks per second for this CPU.
    u64 PerfCountFrequency = SDL_GetPerformanceFrequency();

    // Administration to enforce a framerate of GameUpdateHz
    u64 StartCycleCount = _rdtsc();
    u64 StartTime = SDL_GetPerformanceCounter();

    while (GameState.Running)
    {
        ProcessInput(&WindowState, &GameState, &BackBuffer);

        if (GameCodeChanged(&GameLib) > GameLib.LastWriteTime)
        {
            printf("[Info] GameCode has changed, reloading!\n");
            LoadGameCode(&GameLib);
        }
        
        // TODO: simulate game
        GameLib.GameUpdateAndRender(&GameState, &BackBuffer);

        TryWaitForNextFrame(StartTime, TargetSecondsPerFrame);

        // Measure the Game time, ignore SDL Update time.
        u64 EndTime = SDL_GetPerformanceCounter();

        UpdateWindow(&WindowState, BackBuffer.Pixels);

        /// 
        /// Profiling
        ///

        // Cycles
        u64 EndCycleCount = _rdtsc();
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
        printf("Time: %.02fms/f, FPS: %.02ff/s, Cycles: %.02fmc/f\n", MSPF, FPS, MCPF);
        StartCycleCount = EndCycleCount;
        StartTime = EndTime;
    }

    DestroyWindow(&WindowState);
    DestroyBackBuffer(&BackBuffer);
    return 0;
}
