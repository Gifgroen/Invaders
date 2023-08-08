#include <stdio.h>

union v2 
{
    struct 
    {
        int X, Y;
    };
    struct 
    {
        int Width, Height;
    };
};

struct rect 
{
    v2 Origin;
    v2 Size;
};

void Fill(void *Buffer, v2 Size) 
{
    int *Src = (int *)Buffer;
    for (int Y = 0; Y < Size.Height; ++Y)
    {
        int *Row = Src + Y * Size.Width;
        for (int X = 0; X < Size.Width; ++X)
        {
            *(Row + X) = 0;
        }
    }
}

void DrawRect(void *Buffer, v2 BufferSize, rect Rect) 
{
    int *Src = (int *)Buffer;
    v2 Size = Rect.Size;

    int *Row = Src + (Rect.Origin.Y * BufferSize.Width) + Rect.Origin.X;
    for (int Y = 0; Y < Size.Height; ++Y)
    {
        for (int X = 0; X < Size.Width; ++X)
        {
            *(Row + X) = 1;
        }
        Row += BufferSize.Width;
    }
}

void Print(void *Buffer, v2 Size)
{
    int *Src= (int *)Buffer;
    for (int Y = 0; Y < Size.Height; ++Y)
    {
        int *Row = Src + Y * Size.Width;
        for (int X = 0; X < Size.Width; ++X)
        {
            int *Column = Row + X;
            printf("%d", *Column);
        }   
        printf("\n");
    }
}

int main(int Argc, char **Argv)
{
    int Width = 16;
    int Height = 9;
    int Buffer[Height][Width];

    v2 BufferSize = {};
    BufferSize.Width = Width;
    BufferSize.Height = Height;

    printf("Cleared: \n");
    Fill(Buffer, BufferSize);
    Print(Buffer, BufferSize);

    rect R = {};
    v2 Origin = {{8, 3}};
    v2 Size = {{7, 5}};
    R.Origin = Origin;
    R.Size = Size;

    printf("Filled: \n");
    DrawRect(Buffer, BufferSize, R);
    Print(Buffer, BufferSize);

    return 0;
}