#ifndef RENDERING_CAMERA_H
#define RENDERING_CAMERA_H

#include "math/vector3.h"

struct Camera
{
    Vector3 position;
    Vector3 target;
};

#endif