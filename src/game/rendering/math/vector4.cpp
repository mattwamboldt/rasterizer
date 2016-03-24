#include "vector3.h"
#include "vector4.h"
#include <math.h>

Vector4::Vector4(const Vector3& v) :
    x(v.x), y(v.y), z(v.z), w(1.0f)
{}

float Vector4::Length()
{
    return sqrt(x * x + y * y + z * z + w * w);
}

void Vector4::Normalize()
{
    float length = Length();
    x /= length;
    y /= length;
    z /= length;
    w /= length;
}

float Vector4::Dot(const Vector4& v2)
{
    return x * v2.x + y * v2.y + z * v2.z + w * v2.w;
}
