#ifndef RENDER_H
#define RENDER_H

#include "assets.h"
#include "game.h"

struct coordinate_system
{
    v2 Origin;
    v2 XAxis;
    v2 YAxis;

    loaded_texture *Texture;
};

enum render_element_type
{
    element_type_Clear
};

struct render_element
{
    render_element_type Type;
    coordinate_system Basis;
};

struct render_group
{
    u32 ElementIndex;
    render_element Elements[256];
};

#endif // RENDER_H
