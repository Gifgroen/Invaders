#include "os_input.h"
#include "os_window.h"

#include <SDL2/SDL.h>

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

void ProcessInput(window_state *WindowState, game_state *GameState, offscreen_buffer *BackBuffer)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_WINDOWEVENT:
            {
                ProcessWindowEvent(&e.window, WindowState, BackBuffer);
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
