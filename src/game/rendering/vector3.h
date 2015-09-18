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
    Vector3 Cross(const Vector3& v) const;

    friend Vector3 operator+(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }

    friend Vector3 operator-(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    friend Vector3 operator/(const Vector3& v, const float& s)
    {
        return Vector3(v.x / s, v.y / s, v.z / s);
    }

    friend Vector3 operator-(const Vector3& v)
    {
        return Vector3(-(v.x), -(v.y), -(v.z));
    }

    float x;
    float y;
    float z;
};

#endif