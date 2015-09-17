#include "device.h"
#include <float.h>

Device::Device(SDL_Surface* _screen)
    :screen(_screen), renderWidth(screen->w), renderHeight(screen->h)
{
    depthBuffer = new float[renderWidth * renderHeight];
}

Device::~Device()
{
    if (depthBuffer)
    {
        delete[] depthBuffer;
    }
}

// Clears the screen buffer to the given color
void Device::Clear(Color color)
{
    Uint32* pixels = (Uint32 *)screen->pixels;
    Uint32 screenColor = SDL_MapRGBA(screen->format, color.r, color.g, color.b, color.a);

    for (int i = 0; i < renderWidth * renderHeight; ++i)
    {
        pixels[i] = screenColor;
        depthBuffer[i] = FLT_MAX;
    }
}

// Draws a pixel to the screen ignoring the depthbuffer
void Device::PutPixel(int x, int y, Color c)
{
    Uint32* pixels = (Uint32 *)screen->pixels;
    Uint32 index = x + y * renderWidth;
    pixels[index] = SDL_MapRGBA(screen->format, c.r, c.g, c.b, c.a);
}

// Draws a pixel to the screen only if it passes our depth buffer test
void Device::PutPixel(int x, int y, float z, Color c)
{
    Uint32* pixels = (Uint32 *)screen->pixels;
    Uint32 index = x + y * renderWidth;
    if (depthBuffer[index] < z)
    {
        return;
    }

    depthBuffer[index] = z;
    pixels[index] = SDL_MapRGBA(screen->format, c.r, c.g, c.b, c.a);
}

// Draws a point to the screen if it is within the viewport
void Device::DrawPoint(Vector3 point, Color color)
{
    // Clipping what's visible on screen
    if (point.x >= 0 && point.y >= 0 && point.x < renderWidth && point.y < renderHeight)
    {
        // Drawing a point
        PutPixel((int)point.x, (int)point.y, point.z, color);
    }
}

void Device::DrawPoint(int x, int y, const Color& c)
{
    if (x >= 0 && x < screen->w && y >= 0 && y < screen->h)
    {
        PutPixel(x, y, c);
    }
}

Vector3 Device::Project(const Vector3& v, const Matrix& transform) const
{
    Vector3 projectedVector = transform.Transform(v);
    return Vector3(
        ((screen->w / 2) * projectedVector.x) + (screen->w / 2),
        -(((screen->h / 2) * projectedVector.y) - (screen->h / 2)),
        projectedVector.z );
}