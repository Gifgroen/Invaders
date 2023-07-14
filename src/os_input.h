#ifndef OS_INPUT_H
#define OS_INPUT_H

#include <SDL2/SDL.h>
#include "game.h"

struct game_button_state
{
    bool IsDown;
    u32 HalfTransitionCount;
};

struct game_controller
{
    bool IsConnected;

    union 
    {
        game_button_state Buttons[4];
        struct 
        {
            game_button_state MoveUp;
            game_button_state MoveRight;
            game_button_state MoveDown;
            game_button_state MoveLeft;
        };
    };
};

struct game_input
{
    game_controller Controllers[1];
};

#endif