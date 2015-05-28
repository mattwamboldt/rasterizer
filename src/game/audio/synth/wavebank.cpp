#include "wavebank.h"
#include "..\types.h"
#include "..\wavefunctions.h"

namespace Audio
{
	double identity(double val) { return val; }
	double square(double val) { return val * val; }

	struct WaveGenParams{
		WaveGenParams(int numHarmonics, double amp, double harmonicFreq, ValueGenerator waveFunction, ScaleFunc ampScale)
			:numHarmonics(numHarmonics), amp(amp), harmonicFreq(harmonicFreq), waveFunction(waveFunction), ampScale(ampScale)
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

	struct Harmonic
	{
		double amplitude;
		double ratio; // 1.0 being the fundamental frequency
	};

	typedef vector<Harmonic> HarmonicSeries;

	// This code generates harmonics of sine waves which are added together
	// for synthesizing other wave forms with less aliasing 

	// Harmonics are mixed using the formula SUM(0..n) = 1 / (2n - 1) ^ 2
	// Basically we adjust all amplitudes relative to the fundamental then normalize to prevent clipping
	// We also calulate the ratios of the harmonics alongside the amplitudes
	void WaveBank::AddWave(WaveType waveType, double harmonicity, double frequency, double volume)
	{
		WaveTable wave;
		wave.SetFrequency(frequency);
		wave.SetVolume(volume);

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
			harmonic += waveParams[waveType].harmonicFreq * harmonicity;
		}

		wave.Normalize();
		waves.push_back(wave);
	}

	void WaveBank::Write(PCM16* buffer, int count)
	{
		int numWaves = waves.size();
		memset(buffer, 0, count * 2);

		for (int i = 0; i < count; ++i)
		{
			double amplitude = 1.0;
			if (amplitudeEnv)
			{
				amplitude = amplitudeEnv->NextSample();
			}

			double sample = 0.0;
			if (frequencyMod)
			{
				double frequency = frequencyMod->NextSample();
				for (int j = 0; j < numWaves; ++j)
				{
					sample += waves[j].NextSample(frequency);
				}
			}
			else
			{
				for (int j = 0; j < numWaves; ++j)
				{
					sample += waves[j].NextSample();
				}
			}

			buffer[i] = sample * amplitude * 32767;
		}
	}

	void WaveBank::Write(const char* path)
	{
		SDL_RWops* file = SDL_RWFromFile(path, "w");
		if (file)
		{
			Uint32 numSamples = duration * samplingRate;
			PCM16* buffer = new PCM16[numSamples];

			Write(buffer, numSamples);

			if (frequencyMod)
			{
				frequencyMod->ResetStream();
			}

			if (amplitudeEnv)
			{
				amplitudeEnv->ResetStream();
			}

			SDL_RWwrite(file, buffer, 2, numSamples);
			delete buffer;
		}
	}

	void WaveBank::Write(WaveFile& file)
	{
		Uint32 numSamples = duration * samplingRate;
		PCM16* buffer = new PCM16[numSamples];
		Write(buffer, numSamples);

		if (frequencyMod)
		{
			frequencyMod->ResetStream();
		}

		if (amplitudeEnv)
		{
			amplitudeEnv->ResetStream();
		}

		file.Append(buffer, numSamples);
		delete buffer;
	}
}