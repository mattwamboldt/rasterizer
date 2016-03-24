#ifndef RENDERING_SVG_CIRCLE_H
#define RENDERING_SVG_CIRCLE_H

#include "..\device.h"
#include "..\color.h"

// TODO: Make a class for this later
void StrokeCircle(Device* screen, int cx, int cy, Uint32 radius, Color c);
void FillCircle(Device* screen, int cx, int cy, Uint32 radius, Color c);

#endif