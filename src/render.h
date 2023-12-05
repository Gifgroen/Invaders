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
    element_type_Clear = 1,
    element_type_Outline = 2,
};

struct rect_element
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

struct render_group
{
    u32 ElementsSpaceSize;
    u32 ElementsSpaceUsed;
    void *ElementsBase;
};

#endif // RENDER_H
