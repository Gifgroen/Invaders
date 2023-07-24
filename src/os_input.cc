#include "os_input.h"

#define MAX_CONTROLLER_COUNT 1
global_variable SDL_GameController *ControllerHandles[MAX_CONTROLLER_COUNT];

game_controller *GetKeyboardForIndex(game_input *Input, int Index)
{
    game_controller *Result = &(Input->Keyboards[Index]);
    return Result;
}

game_controller *GetControllerForIndex(game_input *Input, int Index)
{
    game_controller *Result = &(Input->Controllers[Index]);
    return Result;
}

__internal void OpenInputControllers() 
{
    for (int ControllerIndex = 0; ControllerIndex < SDL_NumJoysticks(); ++ControllerIndex)
    {
        if (ControllerIndex == ArrayCount(ControllerHandles)) 
        {
            break;
        }
        if (!SDL_IsGameController(ControllerIndex))
        { 
            continue; 
        }

        ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(ControllerIndex);
    }
}

void ProcessKeyEvent(game_button_state *State, bool Value)
{
    State->IsDown = Value;
    State->HalfTransitionCount++;
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

__internal void ProcessGameControllerButton(game_button_state *OldState, game_button_state *NewState, bool Value)
{
    NewState->IsDown = Value;
    NewState->HalfTransitionCount += ((NewState->IsDown == OldState->IsDown) ? 0 : 1);
}

__internal real32 ProcessGameControllerAxisValue(s16 Value, s16 DeadZoneThreshold)
{
    real32 Result = 0;

    if(Value < -DeadZoneThreshold)
    {
        Result = (real32)((Value + DeadZoneThreshold) / (32768.0f - DeadZoneThreshold));
    }
    else if(Value > DeadZoneThreshold)
    {
        Result = (real32)((Value - DeadZoneThreshold) / (32767.0f - DeadZoneThreshold));
    }

    return(Result);
}

void HandleControllerEvents(game_input *OldInput, game_input *NewInput)
{
    for (int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLER_COUNT; ++ControllerIndex)
    {
        SDL_GameController *Controller = ControllerHandles[ControllerIndex];
        if(Controller != 0 && SDL_GameControllerGetAttached(Controller))
        {
            game_controller *OldController = GetControllerForIndex(OldInput, ControllerIndex);
            game_controller *NewController = GetControllerForIndex(NewInput, ControllerIndex);

            // Sticks
            NewController->StickAverageX = ProcessGameControllerAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX), 1);
            NewController->StickAverageY = -ProcessGameControllerAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY), 1);

            if((NewController->StickAverageX != 0.0f) || (NewController->StickAverageY != 0.0f))
            {
                NewController->IsAnalog = true;
            }

            if(SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_UP))
            {
                NewController->StickAverageY = -1.0f;
                NewController->IsAnalog = false;
            }

            if(SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
            {
                NewController->StickAverageX = 1.0f;
                NewController->IsAnalog = false;
            }

            if(SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_DOWN))
            {
                NewController->StickAverageY = 1.0f;
                NewController->IsAnalog = false;
            }

            if(SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_LEFT))
            {
                NewController->StickAverageX = -1.0f;
                NewController->IsAnalog = false;
            }

            // emulated Stick average for D-pad movement usage
            real32 Threshold = 0.5f;
            ProcessGameControllerButton(&(OldController->MoveUp), &(NewController->MoveUp), NewController->StickAverageY > Threshold);
            ProcessGameControllerButton(&(OldController->MoveRight), &(NewController->MoveRight), NewController->StickAverageX > Threshold);
            ProcessGameControllerButton(&(OldController->MoveDown), &(NewController->MoveDown), NewController->StickAverageY < -Threshold);
            ProcessGameControllerButton(&(OldController->MoveLeft), &(NewController->MoveLeft), NewController->StickAverageX < -Threshold);
        }
    }
}
