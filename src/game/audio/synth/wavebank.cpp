#include "wavebank.h"
#include "..\types.h"
#include "..\wavefunctions.h"

namespace Audio
{
	double identity(double val) { return val; }
	double square(double val) { return val * val; }

	struct WaveGenParams{
		WaveGenParams(int numHarmonics, double amp, double harmonicFreq, ValueGenerator waveFunction, ScaleFunc ampScale)
			:amp(amp), harmonicFreq(harmonicFreq), waveFunction(waveFunction), ampScale(ampScale)
		{}

		double numHarmonics;
		double amp;
		double harmonicFreq;
		ValueGenerator waveFunction;
		ScaleFunc ampScale;
	};

	WaveGenParams waveParams[] = {
		{ 1,   1.0, 2.0, &sin_wave, &identity },
		{ 30,  1.0, 2.0, &sin_wave, &identity },
		{ 12,  1.0, 2.0, &cos_wave, &square },
		{ 12, -1.0, 1.0, &sin_wave, &identity },
		{ 12,  1.0, 1.0, &sin_wave, &identity }
	};

	// This code generates harmonics of sine waves which are added together
	// for synthesizing other wave forms with less aliasing 

	// Harmonics are mixed using the formula SUM(0..n) = 1 / (2n - 1) ^ 2
	// Basically we adjust all amplitudes relative to the fundamental then normalize to prevent clipping
	// We also calulate the ratios of the harmonics alongside the amplitudes
	void WaveBank::AddWave(WaveType waveType)
	{
		WaveTable wave;
		double ampFactor = 1.0;
		double harmonic = 1.0;
		double amp = 1.0;

		double step = TWO_PI / TABLELENGTH;

		for (int i = 0; i < waveParams[waveType].numHarmonics; ++i)
		{
			ampFactor = amp / waveParams[waveType].ampScale(harmonic);
			for (int j = 0; j < TABLELENGTH; ++j)
			{
				wave[j] += ampFactor * waveParams[waveType].waveFunction(step * harmonic * j, 0);
			}
			harmonic += waveParams[waveType].harmonicFreq;
		}

		wave.Normalize();
		waves.push_back(wave);
	}
}