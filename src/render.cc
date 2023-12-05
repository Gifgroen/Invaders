#include "render.h"

#include "game.h"
#include "math.h"

///
/// Drawing
///

internal_func void DrawRectangle(offscreen_buffer *Buffer, v2 Origin, v2i Size, u32 Color)
{
    // TODO: properly round Origin.X and Origin.Y.
    u32 Row = (u32)Origin.y * Buffer->Size.width;
    u32 Column = (u32)Origin.x;

    u32 *Pixel = (u32 *)Buffer->Pixels + Row + Column;

    for (u32 Y = 0; Y < Size.height; ++Y)
    {
        for (u32 X = 0; X < Size.width; ++X)
        {
            *Pixel++ = Color;
        }
        Pixel += Buffer->Size.width - Size.width;
    }
}

internal_func u32 AlphaBlend(u32 Texel, u32 Pixel)
{
    real32 SA = (real32)((Texel >> 24) & 0xFF);
    real32 SR = (real32)((Texel >> 16) & 0xFF);
    real32 SG = (real32)((Texel >> 8) & 0xFF);
    real32 SB = (real32)((Texel >> 0) & 0xFF);

    real32 DA = (real32)((Pixel >> 24) & 0xFF);
    real32 DR = (real32)((Pixel >> 16) & 0xFF);
    real32 DG = (real32)((Pixel >> 8) & 0xFF);
    real32 DB = (real32)((Pixel >> 0) & 0xFF);

    real32 RSA = SA / 255.0f;
    real32 InvRsa = (1.0f - RSA);
    real32 A = SA * RSA + DA * InvRsa;
    real32 R = SR * RSA + DR * InvRsa;
    real32 G = SG * RSA + DG * InvRsa;
    real32 B = SB * RSA + DB * InvRsa;

    return ((u32)(A + 0.5f) << 24)
        | ((u32)(R + 0.5f) << 16)
        | ((u32)(G + 0.5f) << 8)
        | ((u32)(B + 0.5f) << 0);
}

internal_func void FillCoordinateSystem(offscreen_buffer *Buffer, loaded_texture *Texture, u32 Color, coordinate_system System)
{
    v2 Points[4] = {
        System.Origin,
        System.Origin + System.XAxis,
        System.Origin + System.XAxis + System.YAxis,
        System.Origin + System.YAxis
    };

    real32 xMin = Buffer->Size.width - 1;
    real32 xMax = 0;
    real32 yMin = Buffer->Size.height - 1;
    real32 yMax = 0;
    for (int i = 0; i < ArrayCount(Points); ++i)
    {
        v2 Point = Points[i];
        if (Point.x < xMin)
        {
            xMin = Point.x;
        }
        if (Point.x > xMax)
        {
            xMax = Point.x;
        }
        if (Point.y < yMin)
        {
            yMin = Point.y;
        }
        if (Point.y > yMax)
        {
            yMax = Point.y;
        }
    }

    real32 InvXAxisLengthSq = 1.0f / LengthSq(System.XAxis);
    real32 InvYAxisLengthSq = 1.0f / LengthSq(System.YAxis);

    for (u32 Y = yMin; Y < yMax; ++Y)
    {
        for(u32 X = xMin; X < xMax; ++X)
        {
            u32 *Pixel = (u32*)Buffer->Pixels + Y * Buffer->Size.width + X;
#if 1
    // TODO: this can/should be optimised!
            v2 Point = V2(X, Y);
            v2 d = Point - System.Origin;
            int Edge0 = Dot(d, -Perp(System.XAxis));
            int Edge1 = Dot(d - System.XAxis, -Perp(System.YAxis));
            int Edge2 = Dot(d - System.XAxis - System.YAxis, Perp(System.XAxis));
            int Edge3 = Dot(d - System.YAxis, Perp(System.YAxis));

            if (Edge0 < 0 && Edge1 < 0 && Edge2 < 0 && Edge3 < 0) 
            {
                real32 U = InvXAxisLengthSq * Dot(d, System.XAxis);
                real32 V = InvYAxisLengthSq * Dot(d, System.YAxis);

                Assert( U >= 0.0f && U <= 1.0f );
                Assert( V >= 0.0f && V <= 1.0f );

                s32 X = (s32)((U * (real32)(Texture->Size.width - 1)) + 0.5f);
                s32 Y = (s32)((V * (real32)(Texture->Size.height - 1)) + 0.5f);

                Assert(X >= 0 && X < Texture->Size.width);
                Assert(Y >= 0 && Y < Texture->Size.height);

                u32 *Texel = (u32*)Texture->Pixels + (u32)(Y * Texture->Size.width) + X;
                *Pixel = AlphaBlend(*Texel, *Pixel);
            }
#else 
            *Pixel = Color;
#endif
        }
    }
}

// internal_func void DrawCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System)
// {
//     v2i AxisSize = V2i(8, 8);
//     // Origin
//     DrawRectangle(Buffer, System.Origin, AxisSize, 0x00000000);

//     // X Axis
//     DrawRectangle(Buffer, System.Origin + System.XAxis, AxisSize, 0xFF0000FF);
//     // Y Axis
//     DrawRectangle(Buffer, System.Origin + System.YAxis, AxisSize, 0xFF0000FF);
// }

// internal_func void DrawPointInCoordinateSystem(offscreen_buffer *Buffer, coordinate_system System, v2 Point, u32 Color)
// {
//     DrawRectangle(Buffer, System.Origin + Point.x * System.XAxis + Point.y * System.YAxis, V2i(16, 16), Color);
// }

// internal_func void DrawTexture(offscreen_buffer *Buffer, v2 Origin, loaded_texture *Texture)
// {
//     Assert(Origin.x >= 0 && Origin.x < Origin.x + Texture->Size.width);
//     Assert(Origin.y >= 0 && Origin.y < Origin.y + Texture->Size.height);

//     u32 TextureWidth = Texture->Size.width;
//     u32 TextureHeight = Texture->Size.height;

//     v2i BufferSize = Buffer->Size;

//     u32 *Pixels = (u32 *)Buffer->Pixels + ((u32)Origin.y * BufferSize.width) + (u32)Origin.x;
//     u32 *TexturePixel = (u32*)Texture->Pixels;

//     for (int Y = 0; Y < TextureHeight; ++Y)
//     {
//         for (int X = 0; X < TextureWidth; ++X)
//         {
//             *Pixels = AlphaBlend(*TexturePixel, *Pixels);

//             Pixels++;
//             TexturePixel++;
//         }
//         Pixels += BufferSize.width - TextureWidth;
//     }
// }

internal_func void DrawOutline(offscreen_buffer *Buffer, v2 Origin, v2i Size, u16 Thickness, u32 Color)
{
    // TOP
    v2i HorizontalSize = V2i(Size.width, Thickness);
    DrawRectangle(Buffer, Origin, HorizontalSize, Color);

    v2i VerticalSize = V2i(Thickness, Size.height - 2*Thickness);
    // Left
    DrawRectangle(Buffer, V2(Origin.x, Origin.y + Thickness), VerticalSize, Color);

    // Right
    v2 RightOrigin = V2(Origin.x + Size.width - Thickness, Origin.y);
    DrawRectangle(Buffer,  V2(RightOrigin.x, RightOrigin.y + Thickness) , VerticalSize, Color);

    // Bottom
    v2 BottomOrigin = V2(Origin.x, Origin.y + Size.height - Thickness);
    DrawRectangle(Buffer, BottomOrigin, HorizontalSize, Color);
}

/// 
/// Allocation
/// 

internal_func void PushRenderElement(render_group *Group, render_element Element)
{
    Assert(Group->ElementIndex + 1 < ArrayCount(Group->Elements));
    // Todo: If assert fires we're out of render space;
    // that should increase the buffer instead of STOP!
    Group->Elements[Group->ElementIndex++] = Element;
}

internal_func void PushClearElement(render_group *Group, v2 Origin, v2i Size, u32 Color)
{
    render_element Element = {};
    Element.Type = element_type_Clear;

    coordinate_system Basis = {};
    Basis.Origin = Origin;
    // TODO: Size to Axes...
    // TODO: This wont rotate
    Basis.XAxis = V2(1.0f, 0.0f);
    Basis.YAxis = Perp(Basis.XAxis);

    // Element specific?
    Basis.XAxis *= Size.width;  // Scale
    Basis.YAxis *= Size.height; // Scale
    Element.Basis = Basis;

    rect_spec Spec = {};
    Spec.Color = Color;
    Element.ElementSpec = &Spec;

    PushRenderElement(Group, Element);
}

internal_func void PushOutlineElement(render_group *Group, v2 Origin, v2i Size, u16 Thickness, u32 Color)
{
    render_element Element = {};
    Element.Type = element_type_Outline;

    coordinate_system Basis = {};
    Basis.Origin = Origin;
    // TODO: Size to Axes...
    // TODO: This wont rotate
    Basis.XAxis = V2(1.0f, 0.0f);
    Basis.YAxis = Perp(Basis.XAxis);

    // Element specific
    Basis.XAxis *= Size.width;  // Scale
    Basis.YAxis *= Size.height; // Scale
    Element.Basis = Basis;

    outline_spec Spec = {};
    Spec.Thickness = Thickness;
    Spec.Color = Color;
    Element.ElementSpec = &Spec;

    PushRenderElement(Group, Element);
}

internal_func void RenderToOutput(render_group *Group, offscreen_buffer *Buffer)
{
    for (int ElementIndex = 0; ElementIndex < Group->ElementIndex; ++ElementIndex)
    {
        render_element Element = Group->Elements[ElementIndex];
        switch (Element.Type)
        {
            case element_type_Clear:
            {
                coordinate_system Basis = Element.Basis;
                v2i Size = V2i(Basis.XAxis.width, Basis.YAxis.height);
                rect_spec *Spec = (rect_spec *)Element.ElementSpec;
                DrawRectangle(Buffer, Basis.Origin, Size, Spec->Color);
            } break;

            case element_type_Outline:
            {
                coordinate_system Basis = Element.Basis;
                v2i Size = V2i(Basis.XAxis.width, Basis.YAxis.height);
                outline_spec *Spec = (outline_spec *)Element.ElementSpec;
                DrawOutline(Buffer, Basis.Origin, Size, Spec->Thickness, Spec->Color);
            } break;

            default:
            {
                Assert(false);
            };
        }
    }
}
