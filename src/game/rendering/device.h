#ifndef RENDERING_DEVICE_H
#define RENDERING_DEVICE_H

#include <SDL/SDL.h>
#include "color.h"

class Device
{
public:
    Device(SDL_Surface* _screen)
        :screen(_screen)
    {}

    void PutPixel(int x, int y, Color c = Color(0xFFFFFF))
    {
        Uint32* pixels = (Uint32 *)screen->pixels;
        pixels[x + y * screen->w] = SDL_MapRGBA(screen->format, c.r, c.g, c.b, c.a);
    }

private:
    SDL_Surface* screen;
};

#endif