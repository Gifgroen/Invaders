#include "os_input.h"

#include <SDL2/SDL.h>

void ProcessWindowEvent(SDL_WindowEvent *e)
{
    switch (e->event)
    {
        // Get new dimensions and repaint on window size change
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        {
            printf("Size Changed: (w: %d, h: %d)\n", e->data1, e->data2);
        } break;

        // Repaint on exposure
        case SDL_WINDOWEVENT_EXPOSED:
        {
            printf("Exposed\n");
        } break;

        // Mouse entered window
        case SDL_WINDOWEVENT_ENTER:
        {
            printf("Enter\n");
        } break;
        
        // Mouse left window
        case SDL_WINDOWEVENT_LEAVE:
        {
            printf("Leave\n");
        } break;

        // Window has keyboard focus
        case SDL_WINDOWEVENT_FOCUS_GAINED:
        {
            printf("Focus gained\n");   
        } break;

        // Window lost keyboard focus
        case SDL_WINDOWEVENT_FOCUS_LOST:
        {
            printf("Focus lost\n");
        } break;

        // Window minimized
        case SDL_WINDOWEVENT_MINIMIZED:
        {
            printf("Minimised\n");
        } break;

        // Window maximized
        case SDL_WINDOWEVENT_MAXIMIZED:
        {
            printf("Maximised\n");
        } break;
        
        // Window restored
        case SDL_WINDOWEVENT_RESTORED:
        {
            printf("Restored\n");
        } break;
    }
}

void ProcessKeyboardEvents(SDL_Event *e, game_state *GameState)
{
    switch (e->key.keysym.sym)
    {
        case SDLK_ESCAPE:
        {
            GameState->Running = false;
        } break;
    }
}

void ProcessInput(game_state *GameState)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_WINDOWEVENT:
            {
                ProcessWindowEvent(&e.window);
            } break;

            case SDL_QUIT:
            {
                GameState->Running = false;
            } break;

            case SDL_KEYDOWN:
            {
                ProcessKeyboardEvents(&e, GameState);
            } break;
        }
    }
}
