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

struct rect_spec
{
    u32 Color;
};

struct outline_spec
{
    u16 Thickness;
    u32 Color;
};

struct render_element
{
    render_element_type Type;
    coordinate_system Basis;

    void *ElementSpec;
};

struct render_group
{
    u32 ElementIndex;
    render_element Elements[256];
};

#endif // RENDER_H
