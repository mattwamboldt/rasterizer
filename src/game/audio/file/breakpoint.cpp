#include "breakpoint.h"
#include <fstream>
#include <string>
#include <math.h>

#include "../../debug.h"
#include "../util.h"

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
	
	Breakpoint BreakpointFile::MinPoint()
	{
		Breakpoint min = {0.0f, 0.0f};
		for(int i = 0; i < points.size(); ++i)
		{
			if(min.value > points[i].value)
			{
				min = points[i];
			}
		}

		return min;
	}

	bool BreakpointFile::InRange(float min, float max)
	{
		for(int i = 0; i < points.size(); ++i)
		{
			if(points[i].value > max || points[i].value < min)
			{
				return false;
			}
		}

		return true;
	}

	float BreakpointFile::Value(float time)
	{
		Uint32 numPoints = points.size();
		if(numPoints < 2) return 0.0f;

		if(points[low].time < time)
		{
			low = 0;
			high = 1;
		}
		
		while(high < numPoints && time > points[high].time)
		{
			++low;
			++high;
		}

		if( high == numPoints || time < points[low].time )
		{
			return points[low].value;
		}

		Breakpoint left = points[low];
		Breakpoint right = points[high];

		float span = right.time - left.time;
		if(span == 0.0f)
		{
			return right.value;
		}

		return lerp(left.value, right.value, (time - left.time) / span);
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
