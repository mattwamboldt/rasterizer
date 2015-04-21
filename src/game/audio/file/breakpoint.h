#ifndef AUDIO_FILE_BREAKPOINT
#define AUDIO_FILE_BREAKPOINT

#include <SDL/SDL.h>
#include <vector>

namespace Audio
{
	struct Breakpoint
	{
		float time;
		float value;
	};

	class BreakpointFile
	{
	public:
		Breakpoint MaxPoint();
		Uint32 NumPoints(){ return points.size(); }
		const Breakpoint& At(Uint32 index){ return points[index]; }
		void Scale(float value);

		void GenerateCurve(double duration, Uint32 numPoints, double startVal, double endVal);

		bool Load(const char* path);
		bool Write(const char* path);
	private:
		std::vector<Breakpoint> points;
	};
}

#endif
