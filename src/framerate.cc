#include "framerate.h"

#include <SDL2/SDL.h>

internal real32 GetSecondsElapsed(u64 OldCounter, u64 CurrentCounter)
{
    return ((real32)(CurrentCounter - OldCounter) / (real32)(SDL_GetPerformanceFrequency()));
}

internal void TryWaitForNextFrame(u64 LastCounter, real64 TargetSecondsPerFrame) 
{
    if (GetSecondsElapsed(LastCounter, SDL_GetPerformanceCounter()) < TargetSecondsPerFrame)
    {
        s32 TimeToSleep = ((TargetSecondsPerFrame - GetSecondsElapsed(LastCounter, SDL_GetPerformanceCounter())) * 1000) - 1;
        if (TimeToSleep > 0)
        {
            SDL_Delay(TimeToSleep);
        }

        if (GetSecondsElapsed(LastCounter, SDL_GetPerformanceCounter()) >= TargetSecondsPerFrame) 
        {
            printf("Frame time %02f was more then our target\n", GetSecondsElapsed(LastCounter, SDL_GetPerformanceCounter()));
        }
        while (GetSecondsElapsed(LastCounter, SDL_GetPerformanceCounter()) < TargetSecondsPerFrame)
        {
            // Waiting...
        }
    }
}