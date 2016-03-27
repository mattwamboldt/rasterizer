#include "matrix.h"
#include <math.h>
#include "../../debug.h"

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
    float i1j1, float i1j2, float i1j3, float i1j4,
    float i2j1, float i2j2, float i2j3, float i2j4,
    float i3j1, float i3j2, float i3j3, float i3j4,
    float i4j1, float i4j2, float i4j3, float i4j4
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
    return Vector3(values[0][3], values[1][3], values[2][3]);
}

Vector3 Matrix::Transform(const Vector3& v) const
{
    Vector3 result;
    result.x = (v.x * values[0][0]) + (v.y * values[0][1]) + (v.z * values[0][2]);
    result.y = (v.x * values[1][0]) + (v.y * values[1][1]) + (v.z * values[1][2]);
    result.z = (v.x * values[2][0]) + (v.y * values[2][1]) + (v.z * values[2][2]);
    return result;
}

// Matrix transformation is done by multiplying the matrix by a column vector, resulting in another column vector
Vector4 Matrix::Transform(const Vector4& v) const
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
    *this = Identity;
    values[1][1] = values[2][2] = cos(radians);
    values[2][1] = sin(radians);
    values[1][2] = -values[2][1];
}

void Matrix::BuildRotationY(const float radians)
{
    *this = Identity;
    values[0][0] = values[2][2] = cos(radians);
    values[0][2] = sin(radians);
    values[2][0] = -values[0][2];
}

void Matrix::BuildRotationZ(const float radians)
{
    *this = Identity;
    values[0][0] = values[1][1] = cos(radians);
    values[1][0] = sin(radians);
    values[0][1] = -values[1][0];
}

// yaw = rotate y, pitch = rotate x, roll = rotate z
void Matrix::BuildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians)
{
    Matrix yawMat;
    yawMat.BuildRotationY(yawRadians);

    Matrix pitchMat;
    pitchMat.BuildRotationX(pitchRadians);

    Matrix rollMat;
    rollMat.BuildRotationZ(rollRadians);

    *this = yawMat * (pitchMat * rollMat);
}

// Note the order of the cross products matter so if things are upside down or
// facing the wrong way it's cause something is backwards here
void Matrix::BuildLookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
{
    // Calculates forward in camera space, we're going to use negative z as our forward
    // I think it's dumb but it's what opengl uses and we've followed it so far
    Vector3 zaxis = eye - at;
    zaxis.Normalize();

    // Calculates left-right in camera space using the up vector
    // Works because the cross product gives us a perpendicular vector
    Vector3 xaxis = up.Cross(zaxis);
    xaxis.Normalize();

    // Now that we know forward and left right, we can get up in camera space
    // using another cross product
    Vector3 yaxis = zaxis.Cross(xaxis);
    yaxis.Normalize();

    // To construct the matrix we could figure out the angles between our new axes and
    // origin then make a rotation matrix from those angles and concatenate that with
    // a translation matrix to make the camera the origin....

    // OR We can take advantage of this new space being orthonormal and just shove
    // everything in our matrix like so:

    Vector3 nEye = -eye;
    *this = Identity;
    values[0][0] = xaxis.x; values[0][1] = xaxis.y; values[0][2] = xaxis.z; values[0][3] = nEye.Dot(xaxis);
    values[1][0] = yaxis.x; values[1][1] = yaxis.y; values[1][2] = yaxis.z; values[1][3] = nEye.Dot(yaxis);
    values[2][0] = zaxis.x; values[2][1] = zaxis.y; values[2][2] = zaxis.z; values[2][3] = nEye.Dot(zaxis);
}

// The projection matrices are based on the opengl ones and therfore have all of it's quirks and benefits
void Matrix::BuildOrthographicProjection(float bottom, float top, float left, float right, float near, float far)
{
    values[0][0] = 2.0f / (right - left);
    values[1][1] = 2.0f / (top - bottom);
    values[2][2] = -2.0f / (far - near);

    values[0][3] = -(right + left) / (right - left);
    values[1][3] = -(top + bottom) / (top - bottom);
    values[2][3] = -(far + near) / (far - near);
    values[3][3] = 1;
}

void Matrix::BuildPerspectiveProjection(float fov, float aspect, float near, float far)
{
	float tangent = tanf(fov * 0.5f * (M_PI / 180.0));
	float halfHeight = tangent * near;
	float halfWidth = halfHeight * aspect;
	BuildPerspectiveProjection(-halfHeight, halfHeight, -halfWidth, halfWidth, near, far);
}

void Matrix::BuildPerspectiveProjection(float bottom, float top, float left, float right, float near, float far)
{
    values[0][0] = (2 * near) / (right - left);
    values[1][1] = (2 * near) / (top - bottom);
    
    values[0][2] = (right + left) / (right - left);
    values[1][2] = (top + bottom) / (top - bottom);
    values[2][2] = -(far + near) / (far - near);
    values[3][2] = -1;
    
    values[2][3] = (-2.0f * far * near) / (far - near);
}

void Matrix::console() const
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            Debug::console("%f ", values[i][j]);
        }
        Debug::console("\n");
    }
}

Matrix operator * (const Matrix& a, const Matrix& b)
{
    Matrix result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result.values[i][j] = a.values[i][0] * b.values[0][j];
            result.values[i][j] += a.values[i][1] * b.values[1][j];
            result.values[i][j] += a.values[i][2] * b.values[2][j];
            result.values[i][j] += a.values[i][3] * b.values[3][j];
        }
    }

    return result;
}