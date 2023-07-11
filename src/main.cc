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

        UpdateWindow(&WindowState, BackBuffer.Pixels);
    }

    DestroyWindow(&WindowState);
    DestroyBackBuffer(&BackBuffer);
    return 0;
}
