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

#include "defs.h"
#include "types.h"
#include "game.h"

#include "framerate.cc"
#include "gamelib.cc"
#include "math.cc"
#include "os_input.cc"
#include "os_window.cc"

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
    GameMemory.TransientStorage = calloc(GameMemory.TransientStorageSize + GameMemory.PermanentStorageSize, sizeof(u8));
    GameMemory.PermanentStorage = ((u8*)GameMemory.TransientStorage) + GameMemory.TransientStorageSize;

    game_state *GameState = (game_state *)GameMemory.TransientStorage;

    int const GameUpdateHz = 30;
    real32 TargetSecondsPerFrame = 1.0f / (real64)GameUpdateHz;
    int DetectedFrameRate = GetWindowRefreshRate(WindowState.Window);
    if (DetectedFrameRate != GameUpdateHz)
    {
        std::cout << "Device capable refresh rate is " << DetectedFrameRate << " Hz, but Game runs in " << GameUpdateHz << " Hz\n";
    }

    GameState->DeltaTime = 1.f / (real32)GameUpdateHz;

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

    GameLib.GameInit(&GameMemory, &BackBuffer);

    std::cout << "Ok, lets run!" << std::endl;

    while (GameState->Running)
    {
        game_controller *OldKeyboardController = GetKeyboardForIndex(OldInput, 0);
        game_controller *NewKeyboardController = GetKeyboardForIndex(NewInput, 0);
        // *NewKeyboardController = {};
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
                    ProcessKeyboardEvents(&e, GameState, NewKeyboardController);
                } break;
            }
        }
        HandleControllerEvents(OldInput, NewInput);

        if (GameCodeChanged(&GameLib) > GameLib.LastWriteTime)
        {
            std::cout << "[Info] GameCode has changed, reloading!\n" << std::endl;
            LoadGameCode(&GameLib);
        }
        
        // TODO: simulate game
        GameLib.GameUpdateAndRender(&GameMemory, &BackBuffer, NewInput);

        TryWaitForNextFrame(StartTime, TargetSecondsPerFrame);

        // Measure the Game time, ignore SDL Update time.
        u64 EndTime = SDL_GetPerformanceCounter();

        UpdateWindow(&WindowState, BackBuffer.Pixels);

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