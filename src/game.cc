#include "game.h"

#include <stdlib.h>

#include "os_input.h"

void GameUpdateAndRender(game_state *GameState, offscreen_buffer *Buffer, game_input *Input)
{
    if (!GameState->IsInitialised)
    {
        GameState->IsInitialised = true;
    }

    for (u32 i = 0; i < ArrayCount(Input->Keyboards); ++i)
    {
        game_controller *Keyboard = &Input->Keyboards[i];
        game_controller *Controller = &Input->Controllers[i];

        v2 Size = Buffer->Size;
        for (u32 Y = 0; Y < Size.Height; ++Y)
        {
            u32 *Row = (u32*)Buffer->Pixels + (Y * Size.Width);
            for (u32 X = 0; X < Size.Width; ++X)
            {
                u32 *Pixel = Row + X;

                if (Keyboard->MoveUp.IsDown || Controller->MoveUp.IsDown)
                {
                    *Pixel = 0xFF00FF00;
                }
                else if (Keyboard->MoveLeft.IsDown || Controller->MoveLeft.IsDown)
                {
                    *Pixel = 0xFFFF0000;
                }
                else if (Keyboard->MoveDown.IsDown || Controller->MoveDown.IsDown)
                {
                    *Pixel = 0xFFFF00FF;
                }
                else if (Keyboard->MoveRight.IsDown || Controller->MoveRight.IsDown)
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
}
