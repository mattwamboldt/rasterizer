#include "tests.h"
#include <math.h>
#include <algorithm>
#include <time.h>
#include <vector>
#include "color.h"
#include "vector3.h"
#include "matrix.h"
#include "../debug.h"
#include "mesh.h"
#include "camera.h"
#include "device.h"

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
        : x(0.0f), y(0.0f), z(0.0f), color(Color())
    {}

    Vertex(const Vector3& v, Color c = Color())
        : x((int)(v.x)), y((int)(v.y)), z((int)(v.z)), color(c)
    {}

    Vertex(float _x, float _y, float _z, Color c = Color())
        : x((int)(_x)), y((int)(_y)), z((int)(_z)), color(c)
    {}

    float x;
    float y;
    float z;
    Color color;
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

// The midpoint circle algorithm takes advantage of the fact that circles are highly symetrical
// We draw all the octants at once until we pass 45 degrees, adjusting the y coordinate
// using a determinant in the same manner as the bresenham algorithm
void DrawCircle(Device* screen, const Point& origin, Uint32 radius, Color c)
{
    int determinant = 3 - 2 * radius;
    int x = 0;
    int y = radius;

    while (x < y)
    {
        screen->DrawPoint(origin.x + x, origin.y + y, c);
        screen->DrawPoint(origin.x + x, origin.y - y, c);
        screen->DrawPoint(origin.x - x, origin.y + y, c);
        screen->DrawPoint(origin.x - x, origin.y - y, c);
        screen->DrawPoint(origin.x + y, origin.y + x, c);
        screen->DrawPoint(origin.x + y, origin.y - x, c);
        screen->DrawPoint(origin.x - y, origin.y + x, c);
        screen->DrawPoint(origin.x - y, origin.y - x, c);

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

void DrawClock(Device* screen, const Point& origin, Color c)
{
	time_t currtime = time(NULL);
	float currsecond = (currtime % 60) / 60.0f;

	Point edge;
	edge.x = origin.x + 100 * cos(currsecond * 2 * M_PI);
	edge.y = origin.y + 100 * sin(currsecond * 2 * M_PI);
	DrawLineBresenham(screen, origin, edge, c);
    DrawCircle(screen, origin, 100, c);
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

void FillBottomFlatTriangle(Device* screen, const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
    float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
    float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

    // Since we always assume left to right drawing we need the bigger slope to be x2
    // This will still produce the correct triangle because the bounds of the line stay the same
    if (invslope1 > invslope2)
    {
        std::swap(invslope1, invslope2);
    }

    float x1 = v1.x;
    float x2 = v1.x;
    for (int y = v1.y; y <= v2.y; ++y)
    {
        for (int x = (int)x1; x <= x2; ++x)
        {
            screen->DrawPoint(x, y, v1.color);
        }

        x1 += invslope1;
        x2 += invslope2;
    }
}

void FillTopFlatTriangle(Device* screen, const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
    float invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
    float invslope2 = (v3.x - v2.x) / (v3.y - v2.y);

    if (invslope1 < invslope2)
    {
        std::swap(invslope1, invslope2);
    }

    float x1 = v3.x;
    float x2 = v3.x;

    for (int y = v3.y; y > v1.y; --y)
    {
        for (int x = x1; x <= x2; ++x)
        {
            screen->DrawPoint(x, y, v1.color);
        }

        x1 -= invslope1;
        x2 -= invslope2;
    }
}

// The stadard approach for filling a triangle is to actually fill two triangles
// Its easy to draw a flat bottomed or topped triangle, so we split any that are not,
// in half and render them separately
void FillTriangle(Device* screen, Vertex v1, Vertex v2, Vertex v3)
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
        Vertex v4 = Vertex(v1.x + ((v2.y - v1.y) / (v3.y - v1.y)) * (v3.x - v1.x), v2.y, v1.x + ((v2.y - v1.y) / (v3.y - v1.y)) * (v3.z - v1.z));
        FillBottomFlatTriangle(screen, v1, v2, v4);
        FillTopFlatTriangle(screen, v2, v4, v3);
    }
}

Vector3 Project(Device* screen, Vector3 v, const Matrix& transform)
{
    Vector3 projectedVector = transform.Transform(v);
    return Vector3(
        ((screen->Width() / 2) * projectedVector.x) + (screen->Width() / 2),
        -(((screen->Height() / 2) * projectedVector.y) - (screen->Height() / 2)),
        projectedVector.z
    );
}

void DrawMeshCombinedMatrix(Device* screen, const Mesh& mesh, const Matrix& projection, const Matrix& view)
{
    Matrix objectRotation;
    objectRotation.BuildYawPitchRoll(mesh.rotation.y, mesh.rotation.x, mesh.rotation.z);

    Matrix objectTranslation;
    objectTranslation.BuildTranslation(mesh.position);

    Matrix worldMatrix = objectTranslation * objectRotation;

    // At this point our stuff will be in projection space which isn't quite screen space but we need to do a few things before that
    // Also in a right handed system so multiplies go right to left
    Matrix transformMatrix = projection * (view * worldMatrix);

    for (int i = 0; i < mesh.faces.size(); ++i)
    {
        Vector3 v1 = mesh.vertices[mesh.faces[i].a];
        Vector3 v2 = mesh.vertices[mesh.faces[i].b];
        Vector3 v3 = mesh.vertices[mesh.faces[i].c];

        v1 = worldMatrix.Transform(v1);

        Vector3 p1 = Project(screen, mesh.vertices[mesh.faces[i].a], transformMatrix);
        Vector3 p2 = Project(screen, mesh.vertices[mesh.faces[i].b], transformMatrix);
        Vector3 p3 = Project(screen, mesh.vertices[mesh.faces[i].c], transformMatrix);

        float graylevel = 0.25f + (i * 0.75f / mesh.faces.size());
        Uint8 color = (Uint8)(graylevel * 255);
        Color faceColor = Color(color, color, color);

        DrawTriangle(screen, Point(p1.x, p1.y), Point(p2.x, p2.y), Point(p3.x, p3.y), Color(0xFFFFFF));
        FillTriangle(screen, Vertex(p1, faceColor), Vertex(p2, faceColor), Vertex(p3, faceColor));
    }
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

    for (int i = 0; i < mesh.faces.size(); ++i)
    {
        Vector3 v1 = mesh.vertices[mesh.faces[i].a];
        Vector3 v2 = mesh.vertices[mesh.faces[i].b];
        Vector3 v3 = mesh.vertices[mesh.faces[i].c];

        v1 = worldMatrix.Transform(v1);
        v2 = worldMatrix.Transform(v2);
        v3 = worldMatrix.Transform(v3);

        v1 = view.Transform(v1);
        v2 = view.Transform(v2);
        v3 = view.Transform(v3);

        Vector3 p1 = Project(screen, v1, projection);
        Vector3 p2 = Project(screen, v2, projection);
        Vector3 p3 = Project(screen, v3, projection);

        float graylevel = 0.25f + (i * 0.75f / mesh.faces.size());
        Uint8 color = (Uint8)(graylevel * 255);
        Color faceColor = Color(color, color, color);

        DrawTriangle(screen, Point(p1.x, p1.y), Point(p2.x, p2.y), Point(p3.x, p3.y), Color(0xFFFFFF));
        FillTriangle(screen, Vertex(p1, faceColor), Vertex(p2, faceColor), Vertex(p3, faceColor));
    }
}

void Draw(SDL_Surface* _screen, Mesh& mesh)
{
    Device screen(_screen);

    // 3d rendering tests
    Mesh box;
    box.vertices.push_back(Vector3(-1.0f, 1.0f, 1.0f));
    box.vertices.push_back(Vector3(1.0f, 1.0f, 1.0f));
    box.vertices.push_back(Vector3(-1.0f, -1.0f, 1.0f));
    box.vertices.push_back(Vector3(1.0f, -1.0f, 1.0f));
    box.vertices.push_back(Vector3(-1.0f, 1.0f, -1.0f));
    box.vertices.push_back(Vector3(1.0f, 1.0f, -1.0f));
    box.vertices.push_back(Vector3(1.0f, -1.0f, -1.0f));
    box.vertices.push_back(Vector3(-1.0f, -1.0f, -1.0f));

    box.faces.push_back(Face(0, 1, 2));
    box.faces.push_back(Face(1, 2, 3));

    box.faces.push_back(Face(1, 3, 6));
    box.faces.push_back(Face(1, 5, 6));

    box.faces.push_back(Face(0, 1, 4));
    box.faces.push_back(Face(1, 4, 5));

    box.faces.push_back(Face(2, 3, 7));
    box.faces.push_back(Face(3, 6, 7));

    box.faces.push_back(Face(0, 2, 7));
    box.faces.push_back(Face(0, 4, 7));

    box.faces.push_back(Face(4, 5, 6));
    box.faces.push_back(Face(4, 6, 7));

    Camera camera;
    camera.position = Vector3(0.0f, 0.0f, 6.0f);
    camera.target = Vector3(0.0f, 0.0f, 0.0f);

    float rotationsPerSecond = 0.25f;
    float currsecond = ((int)(SDL_GetTicks() * rotationsPerSecond) % 1000) / 1000.0f;
    box.rotation.x = 2 * M_PI * rotationsPerSecond * currsecond;
    box.rotation.y = 2 * M_PI * currsecond;
    mesh.rotation.y = 2 * M_PI * currsecond;

    mesh.position.x = sin(2 * M_PI * currsecond) * 3.0f;

    Matrix viewMatrix;
    viewMatrix.BuildLookAt(camera.position, camera.target, Vector3(0, 1, 0));

    Matrix projectionMatrix;
    projectionMatrix.BuildOrthographicProjection(-3, 3, -4, 4, 0, 10);

    DrawMesh(&screen, mesh, projectionMatrix, viewMatrix);
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