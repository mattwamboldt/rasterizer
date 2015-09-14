#ifndef RENDERING_MATRIX_H
#define RENDERING_MATRIX_H

#include "vector3.h"
#include "vector4.h"

class Matrix
{
public:
    Matrix();
    Matrix(
        float x1y1, float x2y1, float x3y1, float x4y1,
        float x1y2, float x2y2, float x3y2, float x4y2,
        float x1y3, float x2y3, float x3y3, float x4y3,
        float x1y4, float x2y4, float x3y4, float x4y4
    );

    // Fills the translation vector of the matrix
    void SetPosition(const Vector3& pos);
    void SetPosition(const Vector4& pos);

    // Gets out the translation vector of the matrix
    Vector3 GetPosition();

    // Applys the current matrix to the given vector to produce a new one
    Vector3 Transform(const Vector3& v);
    Vector4 Transform(const Vector4& v);

    // Attempts to calculate the inverse of the matrix, not all have one
    Matrix Inverse();

    // Creates a translation matrix from the given position and the identity matrix
    void BuildTranslation(const Vector3& pos);
    void BuildTranslation(const float x, const float y, const float z);

    // Creates a rotation matrix along the given axis
    void BuildRotationX(const float radians);
    void BuildRotationY(const float radians);
    void BuildRotationZ(const float radians);

    // Creates a rotation matrix from yaw pitch and roll components, instead of merging rotations along an axis after the fact
    void BuildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians);
    
    // We'll get to this later
    // Creates a rotation matrix using quaternions (ie. wizard shit)
    // void BuildRotationQuaternio(const Quaternion& q);

    // Uses a location and target to construct a transformation matrix to bring everything into a camera space
    void BuildLookAt(const Vector3& eye, const Vector3& at, const Vector3& up);

    // This matrix is basically 1 in matrix world
    static const Matrix Identity;

    friend Matrix operator * (const Matrix& a, const Matrix& b);

private:
    float values[4][4];
};



#endif