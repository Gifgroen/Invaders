#include "os_window.h"

#include <iostream>

#if defined(PLATFORM_WIN)
#include "SDL.h"
#elif defined(PLATFORM_MACOS)
#include <SDL2/SDL.h>
#else
// TODO: Assert and crash?
#endif

#include "game.h"

bool InitWindow(window_state *WindowState)
{
    u32 InitFlags = SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
    if(SDL_Init(InitFlags) < 0)
    {
        std::cout << "Init failed! " <<  SDL_GetError() << std::endl;
        return false;
    }   

    v2 Size = WindowState->Size;
    u32 WindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    WindowState->Window = SDL_CreateWindow(WindowState->Title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Size.Width, Size.Height, WindowFlags);
    if(WindowState->Window == NULL)
    {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    WindowState->Renderer = SDL_CreateRenderer(WindowState->Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(WindowState->Renderer == NULL)
    {
        std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    WindowState->WindowTexture = SDL_CreateTexture(WindowState->Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, Size.Width, Size.Height);
    if(WindowState->WindowTexture == NULL)
    {
        std::cout << "WindowTexture could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void UpdateOffscreenBuffer(window_state *WindowState, offscreen_buffer *Buffer)
{
    if (WindowState->WindowTexture) 
    {
        SDL_DestroyTexture(WindowState->WindowTexture);
        WindowState->WindowTexture = NULL;
    }

    if (Buffer->Pixels)
    {   
        free(Buffer->Pixels);
    }

    std::cout << "Update BackBuffer to new Size = (" << WindowState->Size.Width << ", " << WindowState->Size.Height << std::endl;
    v2 Size = WindowState->Size;
    u32 Width = Size.Width;
    u32 Height = Size.Height;
    WindowState->WindowTexture = SDL_CreateTexture(WindowState->Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, Width, Height);

    Buffer->Size = Size;

    std::cout << "Texture is " << Width << "x" << Height << std::endl;
    Buffer->Pixels = calloc(Width * Height, sizeof(u32));
}

void ProcessWindowEvent(SDL_WindowEvent *e, window_state *WindowState, offscreen_buffer *BackBuffer)
{
    v2 *NewSize = &WindowState->Size;
    switch (e->event)
    {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        {
            NewSize->Width = e->data1;
            NewSize->Height = e->data2;
            UpdateOffscreenBuffer(WindowState, BackBuffer);
        } break;

        case SDL_WINDOWEVENT_EXPOSED:
        {
            int W, H;
            SDL_GetWindowSize(WindowState->Window, &W, &H);
            NewSize->Width = W;
            NewSize->Height = H;
            UpdateOffscreenBuffer(WindowState, BackBuffer);
        } break;
    }
}

void UpdateWindow(window_state *WindowState, void const *Pixels)
{
    SDL_UpdateTexture(WindowState->WindowTexture, 0, Pixels, WindowState->Size.Width * sizeof(u32));
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
    Buffer->Pixels = NULL;
}