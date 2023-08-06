#include "game.h"

#include <stdlib.h>
#include <iostream>

#include "os_input.h"

void GameInit(game_memory *GameMemory)
{
    game_state *GameState = (game_state*)GameMemory->TransientStorage;
    std::cout << "GameInit: running = " << GameState->Running << std::endl;
}

internal_func void DrawRectangle(offscreen_buffer *Buffer, u32 Color) 
{
    v2 Size = Buffer->Size;
    for (u32 Y = 0; Y < Size.Height; ++Y)
    {
        u32 *Row = (u32*)Buffer->Pixels + (Y * Size.Width);
        for (u32 X = 0; X < Size.Width; ++X)
        {
            u32 *Pixel = Row + X;

            *Pixel = Color;
        }
    }   
}

void GameUpdateAndRender(game_memory *GameMemory, offscreen_buffer *Buffer, game_input *Input)
{
    game_state *GameState = (game_state*)GameMemory->TransientStorage;
    std::cout << "Running -> " << GameState->Running  << std::endl;

    for (u32 i = 0; i < ArrayCount(Input->Keyboards); ++i)
    {
        game_controller *Keyboard = &Input->Keyboards[i];
        game_controller *Controller = &Input->Controllers[i];

        u32 Color;
        if (Keyboard->MoveUp.IsDown || Controller->MoveUp.IsDown)
        {
            Color = 0xFF00FF00;
        }
        else if (Keyboard->MoveLeft.IsDown || Controller->MoveLeft.IsDown)
        {
            Color = 0xFF0000FF;
        }
        else if (Keyboard->MoveDown.IsDown || Controller->MoveDown.IsDown)
        {
            Color = 0xFFFF00FF;
        }
        else if (Keyboard->MoveRight.IsDown || Controller->MoveRight.IsDown)
        {
            Color = 0xFFFF0000;
        } 
        else 
        {
            Color = 0xFFFFFFFF;
        }

        DrawRectangle(Buffer, Color);        
    }
}
