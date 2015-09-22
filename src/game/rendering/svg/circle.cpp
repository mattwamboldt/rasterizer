#include "circle.h"
#include <math.h>

// The midpoint circle algorithm takes advantage of the fact that circles are highly symetrical
// We draw all the octants at once until we pass 45 degrees, adjusting the y coordinate
// using a determinant in the same manner as the bresenham algorithm
void StrokeCircle(Device* screen, int cx, int cy, Uint32 radius, Color c)
{
	int determinant = 3 - 2 * radius;
	int x = 0;
	int y = radius;

	while (x <= y)
	{
		screen->DrawPoint(cx + x, cy + y, c);
		screen->DrawPoint(cx + x, cy - y, c);
		screen->DrawPoint(cx - x, cy + y, c);
		screen->DrawPoint(cx - x, cy - y, c);
		screen->DrawPoint(cx + y, cy + x, c);
		screen->DrawPoint(cx + y, cy - x, c);
		screen->DrawPoint(cx - y, cy + x, c);
		screen->DrawPoint(cx - y, cy - x, c);

		if (determinant < 0)
		{
			determinant += (4 * x) + 6;
		}
		else
		{
			determinant += 4 * (x - y) + 10;
			--y;
		}

		++x;
	}
}

void DrawScanLineClipped(Device* screen, int sx, int ex, int y, Color c)
{
	if (y >= 0 && y < screen->Height())
	{
		sx = SDL_max(sx, 0);
		ex = SDL_min(ex, screen->Width() - 1);

		for (int x = sx; x <= ex; ++x)
		{
			screen->PutPixel(x, y, c);
		}
	}
}

// To draw a circle we use a modified form of the besenham algorithm above
// instead of drawing just points, we draw lines between the points
void FillCircle(Device* screen, int cx, int cy, Uint32 radius, Color c)
{
	int determinant = 3 - 2 * radius;
	int x = 0;
	int y = radius;

	while (x <= y)
	{
		DrawScanLineClipped(screen, cx - x, cx + x, cy + y, c);
		DrawScanLineClipped(screen, cx - x, cx + x, cy - y, c);
		DrawScanLineClipped(screen, cx - y, cx + y, cy + x, c);
		DrawScanLineClipped(screen, cx - y, cx + y, cy - x, c);

		if (determinant < 0)
		{
			determinant += (4 * x) + 6;
		}
		else
		{
			determinant += 4 * (x - y) + 10;
			--y;
		}

		++x;
	}
}
