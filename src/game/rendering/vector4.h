#ifndef RENDERING_VECTOR4_H
#define RENDERING_VECTOR4_H

// The 3d vector is used for normal 3d math, the 4d vector is used for matrix transformations
class Vector3;
class Vector4
{
public:
    Vector4() :
        x(0.0f), y(0.0f), z(0.0f)
    {}

    Vector4(const float _x, const float _y, const float _z, const float _w) :
        x(_x), y(_y), z(_z), w(_w)
    {}

    Vector4(const Vector3& v);

    float Length();
    void Normalize();
    float Dot(const Vector4& v);
    // Convert to 3d for Cross Prodcut

    float x;
    float y;
    float z;
    float w;
};

#endif
