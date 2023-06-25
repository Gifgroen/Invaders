#ifndef OS_WINDOW_H
#define OS_WINDOW_H

#include <SDL2/SDL.h>

struct window_state {
    char const * Title;
    SDL_Window *Window;
    SDL_Surface *Surface;
};

#endif