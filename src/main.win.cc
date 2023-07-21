#include <iostream>

#include "SDL.h"

#include "os_window.cc"

int wmain(int Argc, char *Argv[]) 
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

    // TODO: gamecode as DLL!

    offscreen_buffer BackBuffer = {};
    UpdateOffscreenBuffer(&WindowState, &BackBuffer);

    game_state GameState = {};
    GameState.Running = true;

    while (GameState.Running)
    {        
        std::cout << "SPAMM" << std::endl;
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT:
                {
                    GameState.Running = false;
                } break;
            }
        }
    }

    DestroyWindow(&WindowState);
    DestroyBackBuffer(&BackBuffer);
    return 0;
}