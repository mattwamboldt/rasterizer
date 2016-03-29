#include "tests.h"
#include <math.h>
#include <algorithm>
#include <time.h>
#include <vector>
#include "color.h"
#include "math/vector3.h"
#include "math/matrix.h"
#include "../debug.h"
#include "3d/mesh.h"
#include "camera.h"
#include "device.h"
#include "..\util.h"
#include <sstream>
#include "svg\circle.h"

// This is used to run random softawre rasterizing tests

struct Point
{
    Point() : x(0), y(0) {}
    Point(int _x, int _y) : x(_x), y(_y) {}

    int x;
    int y;
};

// The naive algorithm for drawing a line is to directly plot the line equation
// This will have what we call the jaggies, will be spotty on a slope greater than one and wil draw only one
// dot over and over when at a slope of 0 also it assumes the first point x is less than the second
void DrawLineNaive(Device* screen, const Point& p1, const Point& p2, const Color& c)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    
    for (int x = p1.x; x <= p2.x; ++x)
    {
        screen->DrawPoint(x, p1.y + dx * (x - p1.x) / dy, c);
    }
}

// The Bresenham algorithm is similar to the naive one however it takes into account
// What the change in y is supposed to be and ticks over the y of the line once the
// error accumulates above a certain amount. Since it only works in one octant we
// have to do a bunch of precalculations to set it in the right direction, but it's pretty fast
// and could be done purely with integer math using a low precision of error like 50%
void DrawLineBresenham(Device* screen, const Point& p1, const Point& p2, const Color& c)
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

    // Figure out which axis to move along
    if (yMajor)
    {
        for (int x = startPoint.x; x <= endPoint.x; ++x)
        {
            screen->DrawPoint(y, x, c);

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
            screen->DrawPoint(x, y, c);

            error -= dy;
            if (error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
    }
}

void DrawClockHand(Device* screen, const Point& origin, float percent, int length, Color strokeColor)
{
    // 0% in circle terms is a line going right to 3 o'clock, to make it go up to noon, we add 75%
    percent += 0.75f;

    Point edge;
    edge.x = origin.x + length * cos(percent * 2 * M_PI);
    edge.y = origin.y + length * sin(percent * 2 * M_PI);
    DrawLineBresenham(screen, origin, edge, strokeColor);
}

void DrawClock(Device* screen, const Point& origin, Color strokeColor, Color backingColor)
{
    FillCircle(screen, origin.x, origin.y, 50, backingColor);
    StrokeCircle(screen, origin.x, origin.y, 50, strokeColor);

    time_t currtime = time(NULL);
    tm* localTime = localtime(&currtime);

    const int secondHandLength = 35;
    const int minuteHandLength = 45;
    const int hourHandLength = 25;

    // Render the Second hand
    float currsecond = localTime->tm_sec / 60.0f;
    DrawClockHand(screen, origin, currsecond, secondHandLength, strokeColor);

    // Render the Minute hand
    float currminute = localTime->tm_min / 60.0f;
    DrawClockHand(screen, origin, currminute, minuteHandLength, strokeColor);

    // Render the Hour hand
    float currhour = localTime->tm_hour / 12.0f;
    DrawClockHand(screen, origin, currhour, hourHandLength, strokeColor);
}

void DrawTriangle(Device* screen, const Point& p1, const Point& p2, const Point& p3, Color c)
{
    DrawLineBresenham(screen, p1, p2, c);
    DrawLineBresenham(screen, p2, p3, c);
    DrawLineBresenham(screen, p3, p1, c);
}

void DrawTrapezoid(Device* screen, const Point& origin, Uint32 width, Uint32 height, Uint32 topWidth, Color c)
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

Uint8 lerp(Uint8 min, Uint8 max, float gradient)
{
    return min + (Uint8)((max - min) * gradient);
}

Color lerp(Color start, Color end, float gradient)
{
    return Color(
        lerp(start.r, end.r, gradient),
        lerp(start.g, end.g, gradient),
        lerp(start.b, end.b, gradient),
        lerp(start.a, end.a, gradient)
    );
}

// This function draws a scanline between four vertices that are sorted along the y axis
// It uses multiple lerps to interpolate values in the vertices, such as colour, depth, and texture mapping
// In hardware terms, this would set up and call your pixel shader
void DrawScanline(Device* screen, int y, Vertex va, Vertex vb, Vertex vc, Vertex vd, Color color)
{
    // A and B form a line, C and D form a line
    const Vector3& pa = va.position;
    const Vector3& pb = vb.position;
    const Vector3& pc = vc.position;
    const Vector3& pd = vd.position;

    // We then find out what percentage of the way we are vertically along each line given the y value
    // Note: This isn't the fatest way as these gradients could be found using precomputation and additions
    // that would mean fusing the scanline function with it's containing function or passing way more params
    float gradientLeft = pa.y != pb.y ? (y - pa.y) / (pb.y - pa.y) : 1;
    float gradientRight = pc.y != pd.y ? (y - pc.y) / (pd.y - pc.y) : 1;

    // With those percentages in hand we can use linear interpolation to find the corresponing values along each
    // line for our given y, at a base level we calculate the x co-ordinate for drawing the line
    int startX = (int)lerp(pa.x, pb.x, gradientLeft);
    int endX = (int)lerp(pc.x, pd.x, gradientRight);

    // We also calculate the z values, which are used for depth buffer testing
    float z1 = lerp(pa.z, pb.z, gradientLeft);
    float z2 = lerp(pc.z, pd.z, gradientRight);

    // We also calculate the color values, which are used for lighting an object
    Color c1 = lerp(va.color, vb.color, gradientLeft);
    Color c2 = lerp(vc.color, vd.color, gradientRight);

    // Here's where you'd put various mapping coordinate calculations
    // Things like texture maps, bump maps, normal maps, etc

    // This makes sure we're drawing left to right
    if (startX > endX)
    {
        std::swap(startX, endX);
        std::swap(z1, z2);
        std::swap(c1, c2);
    }

    // Then we draw our pixels, which is the equivalent of a pixel shader
    for (int x = startX; x < endX; ++x)
    {
        float gradientX = (x - startX) / (float)(endX - startX);
        screen->DrawPoint(x, y, lerp(z1, z2, gradientX), lerp(c1, c2, gradientX));
    }
}

// determine on which side of a 2D line a 2D point is
// returns positive values for "right", negative values for "left", and zero if point is on line
float VertexDirection(Vertex p, Vertex start, Vertex end)
{
    return (p.position.x - start.position.x) * (end.position.y - start.position.y) - (end.position.x - start.position.x) * (p.position.y - start.position.y);
}

float LightIntesity(const Vector3& lightSource, const Vector3& position, const Vector3& normal)
{
    Vector3 lightDirection = lightSource - position;
    lightDirection.Normalize();
    return SDL_max(0.0f, normal.Dot(lightDirection));
}

// New algorithm for rasterizing the triangle uses more interpolation to simplify
// editing later values. It draws the whole trangle instead of a top half bottom half like before
void FillTriangle(Device* screen, Vertex v1, Vertex v2, Vertex v3, Vector3& surfaceNormal)
{
    // First we need to vertically sort the vertices so v1 is on top
    if (v2.position.y > v3.position.y)
    {
        std::swap(v2, v3);
    }

    if (v1.position.y > v2.position.y)
    {
        std::swap(v1, v2);
    }

    if (v2.position.y > v3.position.y)
    {
        std::swap(v2, v3);
    }

    // Calculate our lighting values
    Vector3 light(0, 10, 10);
    
    Vector3 centerSurface = (v1.worldPosition + v2.worldPosition + v3.worldPosition) / 3;
    Color faceColor = Color(0xFFFFFF) * LightIntesity(light, centerSurface, surfaceNormal);
    v1.color *= LightIntesity(light, v1.worldPosition, v1.normal);
    v2.color *= LightIntesity(light, v2.worldPosition, v2.normal);
    v3.color *= LightIntesity(light, v3.worldPosition, v3.normal);

    // We draw a right facing triangle one way
    if (VertexDirection(v2, v1, v3) > 0)
    {
        for (int y = (int)v1.position.y; y <= (int)v3.position.y; y++)
        {
            if (y < v2.position.y)
            {
                DrawScanline(screen, y, v1, v3, v1, v2, faceColor);
            }
            else
            {
                DrawScanline(screen, y, v1, v3, v2, v3, faceColor);
            }
        }
    }
    // and a left facing triangle the opposite way
    else
    {
        for (int y = (int)v1.position.y; y <= (int)v3.position.y; y++)
        {
            if (y < v2.position.y)
            {
                DrawScanline(screen, y, v1, v2, v1, v3, faceColor);
            }
            else
            {
                DrawScanline(screen, y, v2, v3, v1, v3, faceColor);
            }
        }
    }
}

Vector3 Project(Device* screen, Vector3 v, const Matrix& transform)
{
    // Trying to prevent weird holes in the geometry by reducing the risk of floating point errors later on
    Vector3 projectedVector = transform.Transform(Vector4(v));
    return Vector3(
        (int)((screen->Width() / 2) * projectedVector.x) + (screen->Width() / 2),
        (int)(-(((screen->Height() / 2) * projectedVector.y) - (screen->Height() / 2))),
        projectedVector.z
    );
}

void DrawMesh(Device* screen, const Mesh& mesh, const Matrix& projection, const Matrix& view)
{
    Matrix objectRotation;
    objectRotation.BuildYawPitchRoll(mesh.rotation.y, mesh.rotation.x, mesh.rotation.z);

    Matrix objectTranslation;
    objectTranslation.BuildTranslation(mesh.position);

    Matrix worldMatrix = objectTranslation * objectRotation;

    // At this point our stuff will be in projection space which isn't quite screen space but we need to do a few things before that
    // Also in a right handed system so multiplies go right to left
    Matrix transformMatrix = projection * (view * worldMatrix);

    // This can be thought of as our vertex shader
    // It'll use the variables available to modify each vertex, before they are passed to the scanline function
    for (int i = 0; i < mesh.faces.size(); ++i)
    {
        Face face = mesh.faces[i];

        // Grab our raw vectors
        Vertex v1 = mesh.vertices[face.a];
        Vertex v2 = mesh.vertices[face.b];
        Vertex v3 = mesh.vertices[face.c];

        // Calculate world space positions, we'll use this later for lighting
        v1.worldPosition = worldMatrix.Transform(v1.position);
        v2.worldPosition = worldMatrix.Transform(v2.position);
        v3.worldPosition = worldMatrix.Transform(v3.position);

        // Also transform the normals to world space for lighting
        v1.normal = objectRotation.Transform(v1.normal);
        v2.normal = objectRotation.Transform(v2.normal);
        v3.normal = objectRotation.Transform(v3.normal);

        // Project the coordinates
        v1.position = Project(screen, v1.position, transformMatrix);
        v2.position = Project(screen, v2.position, transformMatrix);
        v3.position = Project(screen, v3.position, transformMatrix);

        // Finally rasterize the triangle
        FillTriangle(screen, v1, v2, v3, worldMatrix.Transform(face.normal));
    }
}

void Draw(Device* screen, Mesh& mesh)
{
    // 3d rendering tests
    float rotationsPerSecond = 0.25f;
    float currsecond = ((int)(SDL_GetTicks() * rotationsPerSecond) % 1000) / 1000.0f;

    Camera camera;
    camera.position = Vector3(0.0f, 0.0f, 2.0f);
    camera.target = Vector3(0.0f, 0.0f, 0.0f);

    mesh.rotation.y = 2 * M_PI * currsecond;
    //mesh.position.x = sin(2 * M_PI * currsecond) * 3.0f;

    Matrix viewMatrix;
    viewMatrix.BuildLookAt(camera.position, camera.target, Vector3(0, 1, 0));

    // TODO: Figure out how to calculate these values from a camera frustum or something
    Matrix projectionMatrix;
    // projectionMatrix.BuildOrthographicProjection(-3, 3, -4, 4, 0, 2); // Ortho version test
    projectionMatrix.BuildPerspectiveProjection(-3, 3, -4, 4, 1, 100); // Perspective version test

    DrawMesh(screen, mesh, projectionMatrix, viewMatrix);

    DrawClock(screen, Point(55, 55), Color(0xFFFFFFFF), Color(0xFF1c1ccc));
}

/// Stuff to do later
/*
    // If this spinning box left a trail on the points it could be a cool visualizer
    // Like it could scale with amplitude and rotate with frequency or do other cool shit
    Mesh box;
    box.vertices.push_back(Vector3(-1.0f, 1.0f, 1.0f));
    box.vertices.push_back(Vector3(1.0f, 1.0f, 1.0f));
    box.vertices.push_back(Vector3(-1.0f, -1.0f, 1.0f));
    box.vertices.push_back(Vector3(-1.0f, -1.0f, -1.0f));
    box.vertices.push_back(Vector3(-1.0f, 1.0f, -1.0f));
    box.vertices.push_back(Vector3(1.0f, 1.0f, -1.0f));
    box.vertices.push_back(Vector3(1.0f, -1.0f, 1.0f));
    box.vertices.push_back(Vector3(1.0f, -1.0f, -1.0f));

    Camera camera;
    camera.position = Vector3(2.0f, 0.0f, 3.0f);
    camera.target = Vector3(0.0f, 0.0f, 0.0f);

    float currsecond = (SDL_GetTicks() % 1000) / 1000.0f;
    box.rotation.x = 2 * M_PI * currsecond;
    box.rotation.y = 2 * M_PI * currsecond;

    Matrix objectRotation;
    objectRotation.BuildYawPitchRoll(box.rotation.y, box.rotation.x, box.rotation.z);

    Matrix objectTranslation;
    objectTranslation.BuildTranslation(box.position);

    Matrix worldMatrix = objectTranslation * objectRotation;

    Matrix viewMatrix;
    viewMatrix.BuildLookAt(camera.position, camera.target, Vector3(0, 1, 0));

    Matrix projectionMatrix;
    projectionMatrix.BuildOrthographicProjection(4, 3, 0, 20);

    // At this point our stuff will be in projection space which isn't quite screen space but we need to do a few things before that
    // Also in a right handed system so multiplies go right to left
    Matrix transformMatrix = projectionMatrix * viewMatrix * worldMatrix;
    for (int i = 0; i < box.vertices.size(); ++i)
    {
        Vector3 point = transformMatrix.Transform(box.vertices[i]);
        Point screenPoint = Point(
            ((screen->w / 2) * point.x) + (screen->w / 2),
            -(((screen->h / 2) * point.y) - (screen->h / 2))
            );

        if (screenPoint.x >= 0 && screenPoint.x < screen->w && screenPoint.y >= 0 && screenPoint.y < screen->h)
        {
            DrawPoint(screen, screenPoint, Color(0xFFFFFF));
        }
    }
*/