#include "os_input.h"

#include <SDL2/SDL.h>

void ProcessKeyboardEvents(SDL_Event *e, game_state *GameState) 
{
    switch (e->key.keysym.sym)
    {
        case SDLK_ESCAPE: {
            GameState->Running = false;
        } break;
    }
}

void ProcessInput(game_state *GameState) 
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) 
    {
        switch(e.type) { 
            case  SDL_QUIT: {
                GameState->Running = false;
            } break;
            case SDL_KEYDOWN: {
                ProcessKeyboardEvents(&e, GameState);
            } break;
        }
    }
}
