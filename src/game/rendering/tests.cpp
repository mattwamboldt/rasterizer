#include "tests.h"
#include <math.h>
#include <algorithm>

// This is used to run random softawre rasterizing tests

struct Point
{
    Point() : x(0), y(0) {}
    Point(int _x, int _y) : x(_x), y(_y) {}

    int x;
    int y;
};

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

// Useful in very rare occasions but shows the basic idea
void DrawPoint(SDL_Surface* screen, const Point& p, const Color& c)
{
    Uint32* pixels = (Uint32 *)screen->pixels;
    pixels[p.x + p.y * screen->w] = SDL_MapRGB(screen->format, c.r, c.g, c.b);
}

// The naive algorithm for drawing a line is to directly plot the line equation
// This will have what we call the jaggies, will be spotty on a slope greater than one and wil draw only one
// dot over and over when at a slope of 0 also it assumes the first point x is less than the second
void DrawLineNaive(SDL_Surface* screen, const Point& p1, const Point& p2, const Color& c)
{
    Uint32* pixels = (Uint32 *)screen->pixels;
    Uint32 screenColor = SDL_MapRGB(screen->format, c.r, c.g, c.b);
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    
    for (int x = p1.x; x <= p2.x; ++x)
    {
        int y = p1.y + dx * (x - p1.x) / dy;
        pixels[x + y * screen->w] = screenColor;
    }
}

// 
void DrawLineBresenham(SDL_Surface* screen, const Point& p1, const Point& p2, const Color& c)
{
    bool yMajor = p2.y - p1.y > p2.x - p1.x;
    Point startPoint = p1;
    Point endPoint = p2;
     
    // flip our coordinates if the slope is steep
    if (yMajor)
    {
        std::swap(startPoint.x, startPoint.y);
        std::swap(endPoint.x, endPoint.y);
    }

    // swap the points so we can always forward iterate
    if (startPoint.x > endPoint.x)
    {
        std::swap(startPoint, endPoint);
    }

    float dx = endPoint.x - startPoint.x;
    float dy = endPoint.y - startPoint.y;

    float error = dx / 2.0f;
    int ystep = 1;
    if (startPoint.y < endPoint.y)
    {
        ystep = -1;
    }

    int y = startPoint.y;
    int maxX = endPoint.x;

    Uint32* pixels = (Uint32 *)screen->pixels;
    Uint32 screenColor = SDL_MapRGB(screen->format, c.r, c.g, c.b);

    // Figure out which axis to move along
    if (yMajor)
    {
        for (int x = startPoint.x; x <= endPoint.x; ++x)
        {
            pixels[y + x * screen->w] = screenColor;

            error -= dy;
            if (error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
    }
    else
    {
        for (int x = startPoint.x; x <= endPoint.x; ++x)
        {
            pixels[x + y * screen->w] = screenColor;

            error -= dy;
            if (error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
    }
}

void Draw(SDL_Surface* screen)
{
    DrawLineBresenham(screen, Point(50, 50), Point(75, 75), Color(0xFFFFFF));
    DrawLineBresenham(screen, Point(50, 50), Point(75, 50), Color(0xFFFFFF));
    DrawLineBresenham(screen, Point(50, 50), Point(75, 25), Color(0xFFFFFF));
    DrawLineBresenham(screen, Point(50, 50), Point(50, 75), Color(0xFFFFFF));
    DrawLineBresenham(screen, Point(50, 50), Point(50, 25), Color(0xFFFFFF));
    DrawLineBresenham(screen, Point(50, 50), Point(25, 75), Color(0xFFFFFF));
    DrawLineBresenham(screen, Point(50, 50), Point(25, 50), Color(0xFFFFFF));
    DrawLineBresenham(screen, Point(50, 50), Point(25, 25), Color(0xFFFFFF));
}