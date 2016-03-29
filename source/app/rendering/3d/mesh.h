#ifndef RENDERING_MESH_H
#define RENDERING_MESH_H
#include <string>
#include "../math/vector3.h"
#include <vector>
#include "../color.h"

struct Vertex
{
    Vertex(){}

    Vertex(const Vector3& v, Color c = Color(0xFFFFFF))
        : position(v), color(c)
    {}

    Vertex(float _x, float _y, float _z, Color c = Color(0xFFFFFF))
        : position(_x, _y, _z), color(c)
    {}

    Vector3 position;
    Vector3 normal;
    Vector3 worldPosition;
    Color color;
};

struct Face
{
    Face() {}
    Face(int _a, int _b, int _c)
        : a(_a), b(_b), c(_c)
    {}

    int a;
    int b;
    int c;
    Vector3 normal;
};

struct Mesh
{
    // Will read obj format for now
    bool ReadTestFormat(std::string filename);
    void CalculateNormals();
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    Vector3 position;
    Vector3 rotation;
    std::string name;
};

#endif