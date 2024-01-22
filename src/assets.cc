#include "assets.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#if defined(PLATFORM_MACOS)
#include "debug_io.h"
#endif

#include "game.h"

/**
 * LOADING
 */
internal_func loaded_texture LoadTexture(game_memory *GameMemory, char const *Path)
{
    int Width, Height, Comp;

#if defined(PLATFORM_MACOS)
    debug_read_file_result FileResult = GameMemory->ReadEntireFile(Path);
    Assert(FileResult.ContentSize > 0);
    Assert(FileResult.Content != NULL);
    char unsigned *Contents = (char unsigned *)FileResult.Content;
    unsigned char *Pixels = stbi_load_from_memory(Contents, FileResult.ContentSize, &Width, &Height, &Comp, STBI_rgb_alpha);
#elif defined(PLATFORM_WIN)
    unsigned char *Pixels = stbi_load(Path, &Width, &Height, &Comp, STBI_rgb_alpha);
#endif
    
    if (!Pixels)
    {
#if defined(PLATFORM_MACOS)
        GameMemory->FreeFileMemory(FileResult.Content);
#endif

        loaded_texture Result = {};
        Result.Size.width = 0;
        Result.Size.height = 0;
        Result.Pixels = NULL;
        return Result;
    }

    loaded_texture Result = {};
    v2 Size = {};
    Size.width = Width;
    Size.height = Height;
    Result.Size = Size;
    Result.Pixels = Pixels;

#if defined(PLATFORM_MACOS)
    GameMemory->FreeFileMemory(FileResult.Content);
#endif

    return Result;
}

/**
 * Allocation
 */
internal_func loaded_texture CreateTextureAsset(game_memory *Memory, assets *Assets, char const *Path)
{
    char const *BasePath = Assets->BasePath;
    // TODO: Search for assets from . instead of hardcoded steps up and hardcoded data folder
    local_persist char const *Relative = "../../data/";

    memory_size const PathLength = strlen(BasePath) + strlen(Relative) + strlen(Path) + 1;
    char ShipPath[PathLength];
    snprintf(ShipPath, PathLength, "%s%s%s", BasePath, Relative, Path);

    loaded_texture Result = LoadTexture(Memory, ShipPath);
    return Result;
}

internal_func void AllocateAssets(game_memory *Memory,assets *Assets)
{
    // TODO: better retrieval through Type? or Tag?
    Assets->Ships[0] = CreateTextureAsset(Memory, Assets, "playerShip1_red.png");
    Assets->Ships[1] = CreateTextureAsset(Memory, Assets, "playerShip2_green.png");
    Assets->Ships[2] = CreateTextureAsset(Memory, Assets, "playerShip3_blue.png");
}
