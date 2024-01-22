#include "render.h"

#include "game.h"
#include "math.h"

///
/// Drawing
///

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

internal_func void DrawClear(offscreen_buffer *Buffer, v2 Origin, v2i Size, u32 Color)
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

internal_func void DrawRectangle(offscreen_buffer *Buffer, coordinate_system System, u32 Color)
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

    for (u32 Y = yMin; Y < yMax; ++Y)
    {
        for(u32 X = xMin; X < xMax; ++X)
        {
            u32 *Pixel = (u32*)Buffer->Pixels + Y * Buffer->Size.width + X;

            // TODO: this can/should be optimised!
            v2 Point = V2(X, Y);
            v2 d = Point - System.Origin;
            int Edge0 = Dot(d, -Perp(System.XAxis));
            int Edge1 = Dot(d - System.XAxis, -Perp(System.YAxis));
            int Edge2 = Dot(d - System.XAxis - System.YAxis, Perp(System.XAxis));
            int Edge3 = Dot(d - System.YAxis, Perp(System.YAxis));

            if (Edge0 < 0 && Edge1 < 0 && Edge2 < 0 && Edge3 < 0) 
            {
                *Pixel = Color;
            }
        }
    }
}

internal_func void DrawOutline(offscreen_buffer *Buffer, v2 Origin, v2i Size, u16 Thickness, u32 Color)
{
    // TOP
    v2i HorizontalSize = V2i(Size.width, Thickness);
    DrawClear(Buffer, Origin, HorizontalSize, Color);

    v2i VerticalSize = V2i(Thickness, Size.height - 2*Thickness);
    // Left
    DrawClear(Buffer, V2(Origin.x, Origin.y + Thickness), VerticalSize, Color);

    // Right
    v2 RightOrigin = V2(Origin.x + Size.width - Thickness, Origin.y);
    DrawClear(Buffer,  V2(RightOrigin.x, RightOrigin.y + Thickness) , VerticalSize, Color);

    // Bottom
    v2 BottomOrigin = V2(Origin.x, Origin.y + Size.height - Thickness);
    DrawClear(Buffer, BottomOrigin, HorizontalSize, Color);
}

internal_func void DrawTexture(offscreen_buffer *Buffer, coordinate_system System, loaded_texture *Texture, u32 Color = 0xff0000ff)
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

///
/// Allocation
///

inline void *PushRenderElement(render_group *Group, memory_size Size)
{
    void *Result = 0;
    Assert(Group->ElementsSpaceUsed + Size < Group->ElementsSpaceSize);
    Result = ((u8 *)Group->ElementsBase) + Group->ElementsSpaceUsed;
    Group->ElementsSpaceUsed += Size;
    return Result;
}

internal_func void PushClearElement(render_group *Group, v2 Origin, v2i Size, u32 Color)
{
    rectangle_element *Element = (rectangle_element*)PushRenderElement(Group, sizeof(rectangle_element));
    Element->Type = element_type_Clear;

    // TODO: this could be a default value somewhere?
    coordinate_system Basis = {};
    Basis.Origin = Origin;
    // TODO: Size to Axes...
    // TODO: This wont rotate
    Basis.XAxis = V2(1.0f, 0.0f);
    Basis.YAxis = Perp(Basis.XAxis);

    Basis.XAxis *= Size.width;  // Scale
    Basis.YAxis *= Size.height; // Scale
    Element->Basis = Basis;

    Element->Color = Color;
}

internal_func void PushRectElement(render_group *Group, v2 Align, v2 Origin, v2i Size, u32 Color, real32 Rotation = 0.0f)
{
    rectangle_element *Element = (rectangle_element*)PushRenderElement(Group, sizeof(rectangle_element));
    Element->Type = element_type_Rectangle;

        // TODO: this could be a default value somewhere?
    coordinate_system Basis = {};
    // Include Rotation
    Basis.XAxis = V2(cosf(Rotation), sinf(Rotation));
    Basis.YAxis = Perp(Basis.XAxis);
    // Include Position and Scale
    Basis.XAxis *= (real32)Size.width;
    Basis.YAxis *= (real32)Size.height;
    Basis.Origin = Origin - Basis.XAxis * Align.x - Basis.YAxis * Align.y; // TODO: figure out and extract hardcoded center align

    Element->Basis = Basis;

    Element->Color = Color;
}

internal_func void PushOutlineElement(render_group *Group, v2 Origin, v2i Size, u16 Thickness, u32 Color, real32 Rotation = 0.0f)
{
    outline_element *Element = (outline_element *)PushRenderElement(Group, sizeof(outline_element));
    Element->Type = element_type_Outline;

    // TODO: this could be a default value somewhere?
    coordinate_system Basis = {};
    // Include Rotation
    Basis.XAxis = V2(cosf(Rotation), sinf(Rotation));
    Basis.YAxis = Perp(Basis.XAxis);
    // Include Position and Scale
    Basis.XAxis *= (real32)Size.width;
    Basis.YAxis *= (real32)Size.height;
    Basis.Origin = Origin;// - Basis.XAxis * 0.5f - Basis.YAxis * 0.5f; // TODO: extract hardcoded center align

    Element->Basis = Basis;

    Element->Thickness = Thickness;
    Element->Color = Color;
}

internal_func void PushTextureElement(render_group *Group, loaded_texture *Texture, v2 Align, v2 Origin, real32 Rotation = 0.0f)
{
    texture_element *Element = (texture_element *)PushRenderElement(Group, sizeof(texture_element));
    Element->Type = element_type_Texture;

    // TODO: this could be a default value somewhere?
    coordinate_system Basis = {};
    // Include Rotation
    Basis.XAxis = V2(cosf(Rotation), sinf(Rotation));
    Basis.YAxis = Perp(Basis.XAxis);
    // Include Position and Scale
    Basis.XAxis *= (real32)Texture->Size.width;
    Basis.YAxis *= (real32)Texture->Size.height;
    Basis.Origin = Origin - Basis.XAxis * Align.x - Basis.YAxis * Align.y; // TODO: figure out and extract hardcoded center align

    Element->Basis = Basis;

    Element->Texture = Texture;
}

internal_func void RenderToOutput(render_group *Group, offscreen_buffer *Buffer)
{
    for (u32 ElementOffset = 0; ElementOffset < Group->ElementsSpaceUsed;)
    {
        render_element_type *Type = (render_element_type *)((u8*)Group->ElementsBase + ElementOffset);
        switch (*Type)
        {
            case element_type_Clear:
            {
                rectangle_element *Element = (rectangle_element *)Type;
                v2i Size = V2i(Element->Basis.XAxis.width, Element->Basis.YAxis.height);
                DrawClear(Buffer, Element->Basis.Origin, Size, Element->Color);

                ElementOffset += sizeof(*Element);
            } break;

            case element_type_Rectangle:
            {
                rectangle_element *Element = (rectangle_element *)Type;
                DrawRectangle(Buffer, Element->Basis, Element->Color);

                ElementOffset += sizeof(*Element);
            } break;

            case element_type_Outline:
            {
                outline_element *Element = (outline_element *)Type;
                coordinate_system Basis = Element->Basis;
                v2i Size = V2i(Basis.XAxis.width, Basis.YAxis.height);
                DrawOutline(Buffer, Basis.Origin, Size, Element->Thickness, Element->Color);

                ElementOffset += sizeof(*Element);
            } break;

            case element_type_Texture:
            {
                texture_element *Element = (texture_element *)Type;
                DrawTexture(Buffer, Element->Basis, Element->Texture);

                ElementOffset += sizeof(*Element);
            } break;

            default:
            {
                Assert(!"Invalid");
            } break;
        }
    }
}
