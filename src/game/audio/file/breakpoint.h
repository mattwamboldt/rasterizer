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
		BreakpointFile() : low(0), high(1) {}
		Breakpoint MaxPoint();
		Breakpoint MinPoint();
		bool InRange(float min, float max);

		Uint32 NumPoints(){ return points.size(); }
		const Breakpoint& At(Uint32 index){ return points[index]; }
		void Scale(float value);

		float Value(float time);

		//These are used when you want to read this like a stream
		//More efficient for sequential access than the value function
		void SetSamplingRate(Uint32 samplesPerSecond);
		void ResetStream();
		float NextSample();

		void Add(const Breakpoint& point) { points.push_back(point); }

		void GenerateCurve(double duration, Uint32 numPoints, double startVal, double endVal);

		bool Load(const char* path);
		bool Write(const char* path);
	private:
		std::vector<Breakpoint> points;
		Uint32 low;
		Uint32 high;

		//used for stream mode
		double currentTime;
		Uint32 left;
		Uint32 right;
		double increment;
		float width;
		float height;
	};
}

#endif
