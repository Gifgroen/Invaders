#include "player_movement.h"

#include "os_input.h"
#include "math.h"

internal_func void SimulatePlayerMovement(game_state *GameState, game_input *Input, v2i BufferSize)
{
    game_controller Keyboard = Input->Keyboards[0];
    game_controller Controller = Input->Controllers[0];

    v2 MovementDirection = {};
    if (Keyboard.MoveLeft.IsDown || Controller.MoveLeft.IsDown)
    {
        MovementDirection.x = -1.f;
    }
    if (Keyboard.MoveRight.IsDown || Controller.MoveRight.IsDown)
    {
        MovementDirection.x = 1.f;
    }
    if (Keyboard.MoveUp.IsDown || Controller.MoveUp.IsDown)
    {
        MovementDirection.y = -1.f;
    }
    if (Keyboard.MoveDown.IsDown || Controller.MoveDown.IsDown)
    {
        MovementDirection.y = 1.f;
    }

    if (MovementDirection.x != 0.f && MovementDirection.y != 0.f)
    {
        MovementDirection *= 0.707106f;
    }

    int PixelsPerMeter = 64;

    // TODO: do this `PixelPerMeter` adjustment later.
    real32 Speed = 10 * PixelsPerMeter; // m/s * pixelsPerMeter = pixels per meter / s
    v2 Acceleration = MovementDirection * Speed;

    // TODO: proper friction to "decelerate"
    Acceleration += -0.9f * GameState->Velocity;

    v2 NewVelocity = Acceleration * GameState->DeltaTime + GameState->Velocity;

    // TODO: Properly wrap around when leavind the screen on the sides?
    v2 NewPlayerP = 0.5f * Acceleration * Square(GameState->DeltaTime) + NewVelocity * GameState->DeltaTime + GameState->PlayerPosition;
    
    if (NewPlayerP.x <= 0)
    {
        NewPlayerP.x = 0;
        NewVelocity.x = 0;
    }
    if (NewPlayerP.x + GameState->PlayerSize.width >= BufferSize.width)
    {
        NewPlayerP.x = BufferSize.width - GameState->PlayerSize.width;
        NewVelocity.x = 0;
    }
    
    if (NewPlayerP.y <= 0)
    {
        NewPlayerP.y = 0;
        NewVelocity.y = 0;
    }
    if (NewPlayerP.y + GameState->PlayerSize.height >= BufferSize.height)
    {
        NewPlayerP.y = BufferSize.height - GameState->PlayerSize.height;
        NewVelocity.y = 0;
    }

    GameState->PlayerPosition = NewPlayerP;
    GameState->Velocity = NewVelocity;

}