#include "os_input.h"

game_controller *GetControllerForIndex(game_input *Input, int Index)
{
    game_controller *Result = &(Input->Controllers[Index]);
    return Result;
}

void ProcessKeyEvent(game_button_state *State, bool IsDown)
{
    State->IsDown = IsDown;
    State->HalfTransitionCount = State->IsDown != IsDown ? 1 : 0;
}

void ProcessKeyboardEvents(SDL_Event *e, game_state *GameState, game_controller *KeyboardInput)
{
    SDL_KeyboardEvent Key = e->key;
    if (Key.repeat != 0) {
        return;
    }

    bool IsDown = Key.state == SDL_PRESSED;

    switch (Key.keysym.sym)
    {
        case SDLK_ESCAPE:
        {
            GameState->Running = false;
        } break;

        case SDLK_UP:
        case SDLK_w:
        {
            ProcessKeyEvent(&(KeyboardInput->MoveUp), IsDown);
        } break;

        case SDLK_LEFT:
        case SDLK_a:
        {
            ProcessKeyEvent(&(KeyboardInput->MoveLeft), IsDown);
        } break;

        case SDLK_DOWN:
        case SDLK_s:
        {
            ProcessKeyEvent(&(KeyboardInput->MoveDown), IsDown);
        } break;

        case SDLK_RIGHT:
        case SDLK_d:
        {
            ProcessKeyEvent(&(KeyboardInput->MoveRight), IsDown);
        } break;

        default: {
            // TODO process...
            printf("Pressed key = %s, repeat = %d\n", SDL_GetKeyName(Key.keysym.sym), Key.repeat);
        } break;
    }
}
