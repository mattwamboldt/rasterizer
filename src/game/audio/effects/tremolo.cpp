#include "tremolo.h"

namespace Audio
{
	Tremolo::Tremolo(float percent, float frequency, WaveType wave)
	{
		osc.SetWaveType(wave);
		osc.SetFrequency(frequency);
		sourceFactor = 1.0 - percent;
		depthFactor = percent * 0.5;
	}

	void Tremolo::Write(PCM16* data, int count)
	{
		for (int i = 0; i < count; ++i)
		{
			data[i] *= sourceFactor + (osc.NextSample() + 1.0) * depthFactor;
		}
	}
}