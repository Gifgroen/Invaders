#ifndef RENDER_H
#define RENDER_H

#include "assets.h"
#include "game.h"

struct coordinate_system
{
    v2 Origin;
    v2 XAxis;
    v2 YAxis;
};

enum render_element_type
{
    element_type_Clear,
    element_type_Outline,
};

struct render_element
{
    render_element_type Type;
    coordinate_system Basis;    // TODO: Texture should be extracted
    u32 Color;                  // TODO: Color should be extraced

    // TODO: Split Style and Position from coords and Color, etc
    u8 Thickness; // This makes no sense for anything other then `render_type_Outline`
};

struct render_group
{
    u32 ElementIndex;
    render_element Elements[256];
};

#endif // RENDER_H
