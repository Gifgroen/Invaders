#include "gamelib.h"

#include <stdlib.h>
#include <stdio.h>

#include <dlfcn.h>
#include <sys/stat.h>

// GameCode
internal s64 GameCodeChanged(game_lib *GameCode) 
{
    char const *Filename = GameCode->LibPath;
    struct stat Result;
    if (stat(Filename, &Result) == 0) 
    {
        return Result.st_mtime;
    }
    return 0;
}

internal int LoadGameCode(game_lib *GameCode)
{
    if (GameCode->LibHandle) 
    {
        dlclose(GameCode->LibHandle);
        GameCode->LibHandle = NULL;
    }

    GameCode->LibHandle = dlopen(GameCode->LibPath, RTLD_LAZY);
    if (!GameCode->LibHandle) 
    {
        printf("Cannot open library: %s\n", dlerror());
        return -1;
    }

    dlerror();  // reset dl errors
    GameCode->GameUpdateAndRender = (GameUpdateAndRender_t)dlsym(GameCode->LibHandle, "GameUpdateAndRender");

    char const *DlSymError = dlerror();
    if (DlSymError) 
    {
        printf("Cannot load required symbol(s) : %s \n", DlSymError);
        dlclose(GameCode->LibHandle);
        return -1;
    }

    GameCode->LastWriteTime = GameCodeChanged(GameCode);

    return 0;
}