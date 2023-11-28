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

internal_func void Clear(offscreen_buffer *Buffer, u32 Color);

internal_func void DrawRectangle(offscreen_buffer *Buffer, v2 Origin, v2i Size, u32 Color);

internal_func u32 AlphaBlend(u32 Texel, u32 Pixel);

internal_func void FillCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System, u32 Color);

internal_func void DrawCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System);

internal_func void DrawPointInCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System, v2 Point, u32 Color);

internal_func void DrawTexture(offscreen_buffer *Buffer, v2 Origin, loaded_texture *Texture);

internal_func void DrawOutline(offscreen_buffer *Buffer, v2 Origin, v2i Size, u16 Thickness, u32 Color);

#endif // RENDER_H
