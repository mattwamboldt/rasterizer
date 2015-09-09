#ifndef RENDERING_VECTOR3_H
#define RENDERING_VECTOR3_H

// The 3d vector is used for normal 3d math, the 4d vector is used for matrix transformations
class Vector4;
class Vector3
{
public:
    Vector3() :
        x(0.0f), y(0.0f), z(0.0f)
    {}

    Vector3(const float _x, const float _y, const float _z) :
        x(_x), y(_y), z(_z)
    {}

    Vector3(const Vector4& v);

    float Length();
    void Normalize();
    float Dot(const Vector3& v);
    Vector3 Cross(const Vector3& v);

    float x;
    float y;
    float z;
};

#endif