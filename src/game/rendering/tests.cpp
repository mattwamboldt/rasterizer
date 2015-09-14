#include "tests.h"
#include <math.h>
#include <algorithm>
#include <time.h>
#include <vector>
#include "color.h"
#include "vector3.h"

typedef std::vector<Vector3> Mesh;

// This is used to run random softawre rasterizing tests

struct Point
{
    Point() : x(0), y(0) {}
    Point(int _x, int _y) : x(_x), y(_y) {}

    int x;
    int y;
};

struct Vertex
{
    Vertex()
        : x(0.0f), y(0.0f), color(0xFFFFFF)
    {}

    Vertex(int _x, int _y, Uint32 argb = 0xFFFFFF)
        : x(_x), y(_y), color(argb)
    {}

    Vertex(int _x, int _y, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a = 255)
        : x(_x), y(_y), color(_r, _g, _b, _a)
    {}

    float x;
    float y;
    Color color;
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

// The Bresenham algorithm is similar to the naive one however it takes into account
// What the change in y is supposed to be and ticks over the y of the line once the
// error accumulates above a certain amount. Since it only works in one octant we
// have to do a bunch of precalculations to set it in the right direction, but it's pretty fast
// and could be done purely with integer math using a low precision of error like 50%
void DrawLineBresenham(SDL_Surface* screen, const Point& p1, const Point& p2, const Color& c)
{
    bool yMajor = abs(p2.y - p1.y) > abs(p2.x - p1.x);
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
	dy = fabs(dy);

    float error = dx / 2.0f;
    int ystep = 1;
    if (startPoint.y > endPoint.y)
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

// The midpoint circle algorithm takes advantage of the fact that circles are highly symetrical
// We draw all the octants at once until we pass 45 degrees, adjusting the y coordinate
// using a determinant in the same manner as the bresenham algorithm
void DrawCircle(SDL_Surface* screen, const Point& origin, Uint32 radius, Color c)
{
    Uint32* pixels = (Uint32 *)screen->pixels;
    Uint32 screenColor = SDL_MapRGB(screen->format, c.r, c.g, c.b);

    int determinant = 3 - 2 * radius;
    int x = 0;
    int y = radius;

    while (x < y)
    {
        pixels[(origin.x + x) + (origin.y + y) * screen->w] = screenColor;
        pixels[(origin.x + x) + (origin.y - y) * screen->w] = screenColor;
        pixels[(origin.x - x) + (origin.y + y) * screen->w] = screenColor;
        pixels[(origin.x - x) + (origin.y - y) * screen->w] = screenColor;
        pixels[(origin.x + y) + (origin.y + x) * screen->w] = screenColor;
        pixels[(origin.x + y) + (origin.y - x) * screen->w] = screenColor;
        pixels[(origin.x - y) + (origin.y + x) * screen->w] = screenColor;
        pixels[(origin.x - y) + (origin.y - x) * screen->w] = screenColor;

        if (determinant < 0)
        {
            determinant += (4 * x) + 6;
        }
        else
        {
            determinant += 4 * (x - y) + 10;
            --y;
        }

        ++x;
    }
}

void DrawClock(SDL_Surface* screen, const Point& origin, Color c)
{
	time_t currtime = time(NULL);
	float currsecond = (currtime % 60) / 60.0f;

	Point edge;
	edge.x = origin.x + 100 * cos(currsecond * 2 * M_PI);
	edge.y = origin.y + 100 * sin(currsecond * 2 * M_PI);
	DrawLineBresenham(screen, origin, edge, c);
    DrawCircle(screen, origin, 100, c);
}

void DrawTriangle(SDL_Surface* screen, const Point& p1, const Point& p2, const Point& p3, Color c)
{
    DrawLineBresenham(screen, p1, p2, c);
    DrawLineBresenham(screen, p2, p3, c);
    DrawLineBresenham(screen, p3, p1, c);
}

void DrawTrapezoid(SDL_Surface* screen, const Point& origin, Uint32 width, Uint32 height, Uint32 topWidth, Color c)
{
    Point ul = Point(origin.x - topWidth / 2, origin.y - height / 2);
    Point ur = Point(origin.x + topWidth / 2, origin.y - height / 2);
    Point ll = Point(origin.x - width / 2, origin.y + height / 2);
    Point lr = Point(origin.x + width / 2, origin.y + height / 2);

    DrawLineBresenham(screen, ul, ur, c);
    DrawLineBresenham(screen, ur, lr, c);
    DrawLineBresenham(screen, ll, lr, c);
    DrawLineBresenham(screen, ll, ul, c);
}


void FillBottomFlatTriangle(SDL_Surface* screen, const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
    Uint32* pixels = (Uint32 *)screen->pixels;
    Uint32 screenColor = SDL_MapRGB(screen->format, v1.color.r, v1.color.g, v1.color.b);

    float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
    float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);
    float x1 = v1.x;
    float x2 = v1.x;
    for (int y = v1.y; y <= v2.y; ++y)
    {
        for (int x = (int)x1; x <= x2; ++x)
        {
            pixels[x + y * screen->w] = screenColor;
        }

        x1 += invslope1;
        x2 += invslope2;
    }
}

void FillTopFlatTriangle(SDL_Surface* screen, const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
    Uint32* pixels = (Uint32 *)screen->pixels;
    Uint32 screenColor = SDL_MapRGB(screen->format, v1.color.r, v1.color.g, v1.color.b);

    float invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
    float invslope2 = (v3.x - v2.x) / (v3.y - v2.y);
    float x1 = v3.x;
    float x2 = v3.x;
    for (int y = v3.y; y > v1.y; --y)
    {
        for (int x = x1; x <= x2; ++x)
        {
            pixels[x + y * screen->w] = screenColor;
        }

        x1 -= invslope1;
        x2 -= invslope2;
    }
}

// The stadard approach for filling a triangle is to actually fill two triangles
// Its easy to draw a flat bottomed or topped triangle, so we split any that are not,
// in half and render them separately
void FillTriangle(SDL_Surface* screen, Vertex v1, Vertex v2, Vertex v3)
{
    // First we need to vertically sort the vertices so v1 is on top
    if (v2.y > v3.y)
    {
        std::swap(v2, v3);
    }

    if (v1.y > v2.y)
    {
        std::swap(v1, v2);
    }

    if (v2.y > v3.y)
    {
        std::swap(v2, v3);
    }

    // Check if we already have a flat bottom or top
    if (v2.y == v3.y)
    {
        FillBottomFlatTriangle(screen, v1, v2, v3);
    }
    else if (v1.y == v2.y)
    {
        FillTopFlatTriangle(screen, v1, v2, v3);
    }
    else
    {
        // Find a vertex to split the triangle
        Vertex v4 = Vertex(v1.x + ((v2.y - v1.y) / (v3.y - v1.y)) * (v3.x - v1.x), v2.y);
        FillBottomFlatTriangle(screen, v1, v2, v4);
        FillTopFlatTriangle(screen, v2, v4, v3);
    }
}

void Draw(SDL_Surface* screen)
{
	DrawClock(screen, Point(screen->w / 2, screen->h / 2), Color(0x5555FF));

    DrawTriangle(screen, Point(50, 50), Point(75, 75), Point(100, 25), Color(0xFF5555));
    DrawTrapezoid(screen, Point(screen->w / 2, screen->h / 4), 150, 50, 100, Color(0xFF55FF));
    FillTriangle(screen, Vertex(50, 550), Vertex(75, 575), Vertex(100, 525));

    // 3d rendering tests
    Mesh box;
    box.push_back(Vector3(-1.0f, 1.0f, 1.0f));
    box.push_back(Vector3(1.0f, 1.0f, 1.0f));
    box.push_back(Vector3(-1.0f, -1.0f, 1.0f));
    box.push_back(Vector3(-1.0f, -1.0f, -1.0f));
    box.push_back(Vector3(-1.0f, 1.0f, -1.0f));
    box.push_back(Vector3(1.0f, 1.0f, -1.0f));
    box.push_back(Vector3(1.0f, -1.0f, 1.0f));
    box.push_back(Vector3(1.0f, -1.0f, -1.0f));

    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
}