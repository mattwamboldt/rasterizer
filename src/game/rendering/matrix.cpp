#include "matrix.h"

// IMPORTANT: Our matrices are stored in row major order, meaning the first index will give us a row
// This means to access element i j of the matrix we use values[i][j]

// We use column vectors like OpenGL because thats what most of the examples on the web use
// That means the translation lives on the last column of the matrix, j = 3

const Matrix Matrix::Identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

Matrix::Matrix()
{
    values[0][0] = 0; values[1][0] = 0; values[2][0] = 0; values[3][0] = 0;
    values[0][1] = 0; values[1][1] = 0; values[2][1] = 0; values[3][1] = 0;
    values[0][2] = 0; values[1][2] = 0; values[2][2] = 0; values[3][2] = 0;
    values[0][3] = 0; values[1][3] = 0; values[2][3] = 0; values[3][3] = 0;
}

Matrix::Matrix(
    float i1j1, float i2j1, float i3j1, float i4j1,
    float i1j2, float i2j2, float i3j2, float i4j2,
    float i1j3, float i2j3, float i3j3, float i4j3,
    float i1j4, float i2j4, float i3j4, float i4j4
    )
{
    values[0][0] = i1j1; values[0][1] = i1j2; values[0][2] = i1j3; values[0][3] = i1j4;
    values[1][0] = i2j1; values[1][1] = i2j2; values[1][2] = i2j3; values[1][3] = i2j4;
    values[2][0] = i3j1; values[2][1] = i3j2; values[2][2] = i3j3; values[2][3] = i3j4;
    values[3][0] = i4j1; values[3][1] = i4j2; values[3][2] = i4j3; values[3][3] = i4j4;
}

void Matrix::SetPosition(const Vector3& pos)
{
    values[0][3] = pos.x;
    values[1][3] = pos.y;
    values[2][3] = pos.z;
    values[3][3] = 1.0f;
}

void Matrix::SetPosition(const Vector4& pos)
{
    values[0][3] = pos.x;
    values[1][3] = pos.y;
    values[2][3] = pos.z;
    values[3][3] = pos.w;
}

Vector3 Matrix::GetPosition()
{
    return Vector3(values[0][1], values[1][3], values[2][3]);
}

Vector3 Matrix::Transform(const Vector3& v)
{
    // Transforming the vec3 requires converting to homogeneous coordinates first
    // That's done implicitly by a constructor in the vec4 class
    Vector4 v4 = Transform(v);
    return Vector3(v4.x / v4.w, v4.y / v4.w, v4.z / v4.w);
}

// Matrix transformation is done by multiplying the matrix by a column vector, resulting in another column vector
Vector4 Matrix::Transform(const Vector4& v)
{
    Vector4 result;
    result.x = (v.x * values[0][0]) + (v.y * values[0][1]) + (v.z * values[0][2]) + (v.w * values[0][3]);
    result.y = (v.x * values[1][0]) + (v.y * values[1][1]) + (v.z * values[1][2]) + (v.w * values[1][3]);
    result.z = (v.x * values[2][0]) + (v.y * values[2][1]) + (v.z * values[2][2]) + (v.w * values[2][3]);
    result.w = (v.x * values[3][0]) + (v.y * values[3][1]) + (v.z * values[3][2]) + (v.w * values[3][3]);
    return result;
}

Matrix Matrix::Inverse()
{
    // TODO: Implement this
    return Matrix();
}

void Matrix::BuildTranslation(const Vector3& pos)
{
    *this = Identity;
    values[0][3] = pos.x;
    values[1][3] = pos.y;
    values[2][3] = pos.z;
}

void Matrix::BuildTranslation(const float x, const float y, const float z)
{
    *this = Identity;
    values[0][3] = x;
    values[1][3] = y;
    values[2][3] = z;
}

void Matrix::BuildRotationX(const float radians)
{
}

void Matrix::BuildRotationY(const float radians)
{
}

void Matrix::BuildRotationZ(const float radians)
{
}


void Matrix::BuildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians)
{
}

void Matrix::BuildLookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
{
}

Matrix operator * (const Matrix& a, const Matrix& b)
{
    Matrix result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            
        }
    }

    return result;
}