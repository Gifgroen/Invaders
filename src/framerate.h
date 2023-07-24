#ifndef FRAMERATE_H
#define FRAMERATE_H

#include "defs.h"

__internal real32 GetSecondsElapsed(u64 OldCounter, u64 CurrentCounter);

__internal void TryWaitForNextFrame(u64 LastCounter, real64 TargetSecondsPerFrame) ;

#endif // FRAMERATE_H