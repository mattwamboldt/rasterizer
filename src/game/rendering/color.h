#ifndef RENDERING_COLOR_H
#define RENDERING_COLOR_H

#include <SDL/SDL.h>

struct Color
{
    Color()
    {
        r = 0;
        g = 0;
        b = 0;
        a = 255;
    }

    Color(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a = 255)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    Color(Uint32 argb)
    {
        r = (argb >> 16) & 255;
        g = (argb >> 8) & 255;
        b = argb & 255;
        a = (argb >> 24) & 255;
    }

    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};

#endif