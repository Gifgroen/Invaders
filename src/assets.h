#ifndef ASSETS_H
#define ASSETS_H

#include "defs.h"
#include "types.h"

struct loaded_texture 
{
    v2 Size;
    void *Pixels;
};

internal_func loaded_texture LoadTexture(char const *Path);

#endif // ASSETS_H