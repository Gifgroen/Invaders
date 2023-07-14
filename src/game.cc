#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#include "os_input.h"

void GameUpdateAndRender(game_state *GameState, offscreen_buffer *Buffer, game_input *Input)
{
    game_controller *KeyboardController = &Input->Controllers[0];

    v2 Size = Buffer->Size;
    for (int Y = 0; Y < Size.Height; ++Y)
    {
        u32 *Row = (u32*)Buffer->Pixels + (Y * Size.Width);
        for (int X = 0; X < Size.Width; ++X)
        {
            u32 *Pixel = Row + X;
            
            if (KeyboardController->MoveUp.IsDown)
            {
                *Pixel = 0xFF00FF00;
            }
            else if (KeyboardController->MoveLeft.IsDown)
            {
                *Pixel = 0xFFFF0000;
            }
            else if (KeyboardController->MoveDown.IsDown)
            {
                *Pixel = 0xFFFF00FF;
            }
            else if (KeyboardController->MoveRight.IsDown)
            {
                *Pixel = 0xFF0000FF;
            } 
            else 
            {
                *Pixel = 0xFFFFFFFF;
            }
        }
    }
}
