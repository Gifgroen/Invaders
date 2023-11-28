#ifndef ASSETS_H
#define ASSETS_H

#include "defs.h"
#include "types.h"

struct loaded_texture
{
    v2 Size;
    void *Pixels;
};

struct assets
{
    char const *BasePath;

    loaded_texture Ships[3];
};

#endif // ASSETS_H