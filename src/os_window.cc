#include "os_window.h"

#include <SDL2/SDL.h>
#include "game.h"
#include "defs.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

bool InitWindow(window_state *WindowState)
{
    u32 InitFlags = SDL_INIT_VIDEO;
    if(SDL_Init(InitFlags) < 0)
    {
        printf("Init failed! %s\n", SDL_GetError());
        return false;
    }   

    // u32 WindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    u32 WindowFlags = SDL_WINDOW_SHOWN;
    WindowState->Window = SDL_CreateWindow(WindowState->Title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        SCREEN_WIDTH, SCREEN_HEIGHT, WindowFlags);
    if(WindowState->Window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    WindowState->Renderer = SDL_CreateRenderer(WindowState->Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(WindowState->Renderer == NULL)
    {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    WindowState->WindowTexture = SDL_CreateTexture(WindowState->Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);    
    if(WindowState->WindowTexture == NULL)
    {
        printf("WindowTexture could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void UpdateWindow(window_state *WindowState, void const *Pixels)
{
    SDL_UpdateTexture(WindowState->WindowTexture, 0, Pixels, 4);
    SDL_RenderCopy(WindowState->Renderer, WindowState->WindowTexture, 0, 0);
    SDL_RenderPresent(WindowState->Renderer);
}

void DestroyWindow(window_state *WindowState)
{
    SDL_DestroyTexture(WindowState->WindowTexture);
    WindowState->WindowTexture = NULL;

    SDL_DestroyRenderer(WindowState->Renderer);
    WindowState->Renderer = NULL;

    SDL_DestroyWindow(WindowState->Window);
    WindowState->Window = NULL;
    

    SDL_Quit();
}

void DestroyBackBuffer(offscreen_buffer *Buffer)
{
    free(Buffer->Pixels);
    Buffer = NULL;
}