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
    element_type_Rectangle,
    element_type_Outline,
    element_type_Texture,
};

struct rectangle_element
{
    render_element_type Type;

    coordinate_system Basis;
    u32 Color;
};

struct outline_element
{
    render_element_type Type;

    coordinate_system Basis;
    u16 Thickness;
    u32 Color;
};

struct texture_element
{
    render_element_type Type;

    coordinate_system Basis;
    loaded_texture *Texture;
};

struct render_group
{
    u32 ElementsSpaceSize;
    u32 ElementsSpaceUsed;
    u8 *ElementsBase;
};

#endif // RENDER_H
