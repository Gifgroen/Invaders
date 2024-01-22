#ifndef OS_INPUT_H
#define OS_INPUT_H

#include "types.h"

struct game_button_state
{
    bool IsDown;
    u32 HalfTransitionCount;
};

struct game_controller
{
    bool IsConnected;
    bool IsAnalog;
    real32 StickAverageX;
    real32 StickAverageY;

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
    game_controller Keyboards[1];
    game_controller Controllers[1];
};

#endif // OS_INPUT_H
