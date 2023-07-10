#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "defs.h"
#include "os_window.cc"
#include "os_input.cc"
#include "gamelib.cc"

int main(int Argc, char *Args[])
{
#if DEBUG 
    printf("[Debug] %d arguments passed. ", Argc - 1);
#endif

    window_state WindowState = {};
    WindowState.Title = "Invaders must die!";
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
    // TODO: connect this size to InitWindow and add resize/SDL expose capabilities.
    // Note: Add commend line input for -size 1024x768 or similar?
    BackBuffer.Width = 1024;
    BackBuffer.Height = 768;
    BackBuffer.Pixels = (u32*)malloc(BackBuffer.Width * BackBuffer.Height * sizeof(u32));

    game_state GameState = {};
    GameState.Running = true;

    while (GameState.Running)
    {
        ProcessInput(&GameState);

        if (GameCodeChanged(&GameLib) > GameLib.LastWriteTime)
        {
            printf("[Info] GameCode has changed, reloading!\n");
            LoadGameCode(&GameLib);
        }
        
        // TODO: simulate game
        GameLib.GameUpdateAndRender(&GameState, &BackBuffer);

        UpdateWindow(&WindowState, BackBuffer.Pixels);
    }

    DestroyWindow(&WindowState);
    DestroyBackBuffer(&BackBuffer);
    return 0;
}
