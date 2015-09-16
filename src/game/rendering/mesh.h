#ifndef RENDERING_MESH_H
#define RENDERING_MESH_H
#include <string>
#include "vector3.h"
#include <vector>

struct Face
{
    Face(int _a, int _b, int _c)
        : a(_a), b(_b), c(_c)
    {}

    int a;
    int b;
    int c;
};

struct Mesh
{
    std::vector<Vector3> vertices;
    std::vector<Face> faces;
    Vector3 position;
    Vector3 rotation;
};

#endif