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

        int w, h;
        SDL_GetWindowSize(WindowState.Window, &w, &h);

        u32 *Pixels = (u32*)malloc(w * h * sizeof(u32));
        
        // TODO: simulate game
        GameLib.GameUpdateAndRender(&GameState, Pixels, w, h);

        UpdateWindow(&WindowState, Pixels);

        free(Pixels);
    }

    DestroyWindow(&WindowState);
    return 0;
}
