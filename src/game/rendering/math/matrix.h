#ifndef RENDERING_MATRIX_H
#define RENDERING_MATRIX_H

#include "vector3.h"
#include "vector4.h"

class Matrix
{
public:
    Matrix();
    Matrix(
        float i1j1, float i1j2, float i1j3, float i1j4,
        float i2j1, float i2j2, float i2j3, float i2j4,
        float i3j1, float i3j2, float i3j3, float i3j4,
        float i4j1, float i4j2, float i4j3, float i4j4
    );

    // Fills the translation vector of the matrix
    void SetPosition(const Vector3& pos);
    void SetPosition(const Vector4& pos);

    // Gets out the translation vector of the matrix
    Vector3 GetPosition();

    // Applys the current matrix to the given vector to produce a new one
    Vector3 Transform(const Vector3& v) const;
    Vector4 Transform(const Vector4& v) const;

    // Attempts to calculate the inverse of the matrix, not all have one
    Matrix Inverse();

    // Creates a translation matrix from the given position and the identity matrix
    void BuildTranslation(const Vector3& pos);
    void BuildTranslation(const float x, const float y, const float z);

    // Creates a rotation matrix along the given axis
    // The direction of the apparent rotation follows the right hand rule
    void BuildRotationX(const float radians);
    void BuildRotationY(const float radians);
    void BuildRotationZ(const float radians);

    // Creates a rotation matrix from yaw pitch and roll components, instead of merging rotations along an axis after the fact
    // This partially defines what is considered forward in our system
    // We consider z to be the forward back dimension which means
    // yaw = rotate y, pitch = rotate x, roll = rotate z
    void BuildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians);
    
    // We'll get to this later
    // Creates a rotation matrix using quaternions (ie. wizard shit)
    // void BuildRotationQuaternio(const Quaternion& q);

    // Uses a location and target to construct a transformation matrix to bring everything into a camera space
    // The look at matrix finds the direction the camera is looking and calculates a new coordinate system
    // from that initial angle and the up vector. It then adds concatenates a translation to -eye with a rotation
    // into the cameras coordinate system
    void BuildLookAt(const Vector3& eye, const Vector3& at, const Vector3& up);

    // These two functions generate our projection matrix which transforms vetrices from camera space into projection space
    // They use a bounding box like openGL mening they may not be the best for accuracy depending on distance or efficiency
    void BuildOrthographicProjection(float bottom, float top, float left, float right, float near, float far);
    void BuildPerspectiveProjection(float bottom, float top, float left, float right, float near, float far);

    // Used for debugging
    void console() const;
    void Set(int i, int j, float value)
    {
        values[i][j] = value;
    }

    // This matrix is basically 1 in matrix world
    static const Matrix Identity;

    friend Matrix operator * (const Matrix& a, const Matrix& b);

private:
    float values[4][4];
};



#endif