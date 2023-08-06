#ifndef OS_WINDOW_H
#define OS_WINDOW_H

#if defined(PLATFORM_WIN)
#include "SDL.h"
#elif defined(PLATFORM_MACOS)
#include <SDL2/SDL.h>
#else
// TODO: Assert and crash?
#endif

#include "defs.h"
#include "types.h"

struct window_state
{
    char const * Title;
    v2 Size;
    SDL_Window *Window;
    SDL_Renderer *Renderer;

    SDL_Texture *WindowTexture;
};

#endif