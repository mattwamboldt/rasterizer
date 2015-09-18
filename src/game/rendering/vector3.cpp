#include "vector3.h"
#include "vector4.h"

#include <math.h>

Vector3::Vector3(const Vector4& v) :
    x(v.x), y(v.y), z(v.z)
{}

float Vector3::Length()
{
    return sqrt(x * x + y * y + z * z);
}

void Vector3::Normalize()
{
    float length = Length();
    x /= length;
    y /= length;
    z /= length;
}

Vector3 Vector3::Cross(const Vector3& v2) const
{
    Vector3 resultant;
    resultant.x = y * v2.z - v2.y * z;
    resultant.y = z * v2.x - v2.z * x;
    resultant.z = x * v2.y - v2.x * y;
    return resultant;
}

float Vector3::Dot(const Vector3& v2) const
{
    return x * v2.x + y * v2.y + z * v2.z;
}
