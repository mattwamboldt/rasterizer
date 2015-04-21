#include "breakpoint.h"
#include <fstream>
#include <string>
#include <math.h>

#include "../../debug.h"

namespace Audio
{
	Breakpoint BreakpointFile::MaxPoint()
	{
		Breakpoint max = {0.0f, 0.0f};
		for(int i = 0; i < points.size(); ++i)
		{
			if(max.value < points[i].value)
			{
				max = points[i];
			}
		}

		return max;
	}

	void BreakpointFile::Scale(float value)
	{
		for(int i = 0; i < points.size(); ++i)
		{
			points[i].value *= value;
		}
	}

	void BreakpointFile::GenerateCurve(double duration, Uint32 numPoints, double startVal, double endVal)
	{
		double start, end, offset;
		const double smallval = 1.0e-4;

		double step = duration / numPoints;
		double range = endVal - startVal;
		if(range == 0.0) return; //no need to generate data

		if(startVal > endVal)
		{
			start = 1.0;
			end = smallval;
			range = -range;
			offset = endVal;
		}
		else
		{
			start = smallval;
			end = 1.0;
			offset = startVal;
		}

		Breakpoint point;
		double thisstep = 0.0;
		double fac = pow(end / start, 1.0 / numPoints);
		for(int i = 0; i < numPoints; ++i)
		{
			point.time = thisstep;
			point.value = offset + (start * range);
			points.push_back(point);

			start *= fac;
			thisstep += step;
		}

		point.time = thisstep;
		point.value = offset + (start * range);
		points.push_back(point);
	}

	bool BreakpointFile::Load(const char* path)
	{
		points.clear();
		std::fstream file;
		file.open(path, std::fstream::in);
		while(file.good())
		{
			Breakpoint point;
			file >> point.time;
			file >> point.value;
			points.push_back(point);
		}

		file.close();
		return true;
	}

	bool BreakpointFile::Write(const char* path)
	{
		std::fstream file;
		file.open(path, std::fstream::out);
		if(!file.is_open())
		{
			return false;
		}

		for(int i = 0; i < points.size(); ++i)
		{
			file << points[i].time;
			file << ' ';
			file << points[i].value;
			file << std::endl;
		}

		file.close();
		return true;
	}
}
