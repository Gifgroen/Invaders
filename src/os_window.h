#ifndef OS_WINDOW_H
#define OS_WINDOW_H

#include <SDL2/SDL.h>

#include "math.h"

struct window_state 
{
    char const * Title;
    v2 Size;
    SDL_Window *Window;   
    SDL_Renderer *Renderer;

    SDL_Texture *WindowTexture;
};

#endif