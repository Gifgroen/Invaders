#include <stdio.h>
#include <SDL2/SDL.h>

#include "defs.h"
#include "os_window.cc"
#include "os_input.cc"
#include "gamelib.cc"

int main(int Argc, char *Args[])
{
#if DEBUG 
    printf("[Debug] %d arguments passed. ", Argc);
#endif

    window_state WindowState = {};
    WindowState.Title = "Invaders must die!";
    if (InitWindow(WindowState) == false) 
    {
        printf("[Error] Initialising the window failed!\n");
        return 1;
    }

    game_lib GameLib = {};
    GameLib.LibPath = "../build/libgame.so";
    if (LoadGameCode(&GameLib) != 0)
    {
        printf("Loading GameCode failed!\n");
    }
    
    game_state GameState = {};
    GameState.Running = true;

    while (GameState.Running) 
    {
        ProcessInput(&GameState);

        if (GameCodeChanged(&GameLib) > GameLib.LastWriteTime) 
        {
            printf("GameCode has changed, reloading!\n");
            LoadGameCode(&GameLib);
        }

        // TODO: simulate game

        GameLib.GameUpdateAndRender(&GameState);
    }

    DestroyWindow(WindowState);
    return 0;
}
