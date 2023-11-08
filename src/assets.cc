#include "assets.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "debug_io.h"

internal_func loaded_texture LoadTexture(char const *Path) 
{
    debug_read_file_result FileResult = DebugReadEntireFile(Path);

    int Width, Height, Comp;
    char unsigned const *Contents = (char unsigned const *)FileResult.Content;
    char unsigned *Pixels = stbi_load_from_memory(Contents, FileResult.ContentSize, &Width, &Height, &Comp, STBI_rgb_alpha);
    
    loaded_texture Result = {};
    v2 Size = {};
    Size.width = Width;
    Size.height = Height;
    Result.Size = Size;
    Result.Pixels = Pixels;

    // stbi_image_free(Pixels);
    // DebugFreeFileMemory(FileResult.Content);

    return Result;
}