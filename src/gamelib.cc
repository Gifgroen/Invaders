#include "gamelib.h"

#include <iostream>

#if defined(PLATFORM_WIN)
#include <windows.h>
#elif defined(PLATFORM_MACOS)
#include <dlfcn.h>
#include <sys/stat.h>
#else 
// TODO: assert and crash?
#endif

internal_func s64 GameCodeChanged(game_lib *GameCode) 
{
    char const *Filename = GameCode->LibPath;
    struct stat Result;
    if (stat(Filename, &Result) == 0)
    {
        return Result.st_mtime;
    }
    return 0;
}

#if defined(PLATFORM_WIN)
internal_func int LoadGameCode(game_lib *GameCode)
{
    std::cout << "LoadGameCode: at path = " << GameCode->LibPath << std::endl;
    if (GameCode->LibHandle)
    {
        std::cout << "Free." << std::endl;
        FreeLibrary((HMODULE)GameCode->LibHandle);
        GameCode->LibHandle = NULL;
        std::cout << "Cleared loaded Library." << std::endl;
    }

    GameCode->LibHandle = LoadLibrary(GameCode->LibPath);
    if (!GameCode->LibHandle)
    {
        std::cout << "Cannot open library: " << GetLastError() << std::endl;
        return -1;
    }

    GameCode->GameInit = (GameInit_t)GetProcAddress((HMODULE)GameCode->LibHandle, "GameInit");
    GameCode->GameUpdateAndRender = (GameUpdateAndRender_t)GetProcAddress((HMODULE)GameCode->LibHandle, "GameUpdateAndRender");
    GameCode->GameGetSoundsSamples = (GameGetSoundSamples_t)GetProcAddress((HMODULE)GameCode->LibHandle, "GameGetSoundSamples");
    if (GameCode->GameUpdateAndRender == NULL)
    {
        std::cout << "Cannot load required symbol(s): " << GetLastError() << std::endl;
        FreeLibrary((HMODULE)GameCode->LibHandle);
        return -1;
    }
    GameCode->LastWriteTime = GameCodeChanged(GameCode);
    return 0;
}
#elif defined(PLATFORM_MACOS)
internal_func int LoadGameCode(game_lib *GameCode)
{
    if (GameCode->LibHandle)
    {
        dlclose(GameCode->LibHandle);
        GameCode->LibHandle = NULL;
    }

    GameCode->LibHandle = dlopen(GameCode->LibPath, RTLD_LAZY);
    if (!GameCode->LibHandle)
    {
        std::cout << "Cannot open library: " << dlerror() << std::endl;
        return -1;
    }

    dlerror();  // reset dl errors
    GameCode->GameInit = (GameInit_t)dlsym(GameCode->LibHandle, "GameInit");
    GameCode->GameUpdateAndRender = (GameUpdateAndRender_t)dlsym(GameCode->LibHandle, "GameUpdateAndRender");
    GameCode->GameGetSoundSamples = (GameGetSoundSamples_t)dlsym(GameCode->LibHandle, "GameGetSoundSamples");

    char const *DlSymError = dlerror();
    if (DlSymError)
    {
        std::cout << "Cannot load required symbol(s): " << DlSymError << std::endl;
        dlclose(GameCode->LibHandle);
        return -1;
    }

    GameCode->LastWriteTime = GameCodeChanged(GameCode);

    return 0;
}
#else 
// Assert and crash?
#endif