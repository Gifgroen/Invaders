#ifndef FRAMERATE_H
#define FRAMERATE_H

#include "defs.h"
#include "types.h"

internal_func real32 GetSecondsElapsed(u64 OldCounter, u64 CurrentCounter);

internal_func void TryWaitForNextFrame(u64 LastCounter, real64 TargetSecondsPerFrame) ;

#endif // FRAMERATE_H