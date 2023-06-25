#include "os_window.h"

#include <SDL2/SDL.h>
#include "defs.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

bool InitWindow(window_state WindowState)
{
    u32 InitFlags = SDL_INIT_VIDEO;
    if(SDL_Init(InitFlags) < 0)
    {
        printf("Init failed!");
        return false;
    }   

    WindowState.Window = SDL_CreateWindow(WindowState.Title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(WindowState.Window == NULL)
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    
    WindowState.Surface = SDL_GetWindowSurface(WindowState.Window);

    return true;
}

void DestroyWindow(window_state WindowState)
{
    SDL_FreeSurface(WindowState.Surface);
    WindowState.Surface = NULL;

    SDL_DestroyWindow(WindowState.Window);
    WindowState.Window = NULL;

    SDL_Quit();
}