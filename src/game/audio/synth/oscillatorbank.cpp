#include "oscillatorbank.h"

namespace Audio
{
	void OscillatorBank::Init(Uint32 numsignals, WaveType wavetype)
	{
		oscillators.clear();
		amplitudes.clear();
		frequencies.clear();

		numOscilltors = numsignals;

		// This code generates harmonics of sine waves which are added together
		// for synthesizing other wave forms with less aliasing 
		
		// Harmonics are mixed using the formula SUM(0..n) = 1 / (2n - 1) ^ 2
		// Basically we adjust all amplitudes relative to the fundamental
		// Then divide all amplitudes by how much we've gone over when those adjusted amplitudes are summed
		// We also calulate the ratios of the harmonics alongside the amplitudes
		double ampFactor = 1.0;
		double freqFactor = 1.0;
		double ampAdjust = 0.0;

		Oscillator temp;

		switch(wavetype)
		{
		case SQUARE_WAVE:
			for(int i = 0; i < numOscilltors; ++i)
			{
				ampFactor = 1.0 / freqFactor;
				amplitudes.push_back(ampFactor);
				frequencies.push_back(freqFactor);
				freqFactor += 2.0;
				ampAdjust += ampFactor;
			}
			break;

		case TRIANGLE_WAVE:
			for(int i = 0; i < numOscilltors; ++i)
			{
				ampFactor = 1.0 / (freqFactor * freqFactor);
				amplitudes.push_back(ampFactor);
				frequencies.push_back(freqFactor);
				freqFactor += 2.0;
				ampAdjust += ampFactor;
			}
			temp.Reset(0.25);
			break;

		case UPSAW_WAVE:
		case DOWNSAW_WAVE:
			for(int i = 0; i < numOscilltors; ++i)
			{
				ampFactor = 1.0 / freqFactor;
				amplitudes.push_back(ampFactor);
				frequencies.push_back(freqFactor);
				freqFactor += 1.0;
				ampAdjust += ampFactor;
			}
			if(wavetype == UPSAW_WAVE)
				ampAdjust = -ampAdjust; // inverts the wave
			break;
		}

		//This adjusts the amplitudes to prevent clipping
		for(int i = 0; i < numOscilltors; ++i)
		{
			amplitudes[i] /= ampAdjust;
			oscillators.push_back(temp);
		}
	}

	void OscillatorBank::WriteTone(WaveFile& file)
	{
		Uint32 numSamples = samplingRate * duration;
		PCM16* buffer = new PCM16[numSamples];
		memset(buffer, 0, numSamples * 2);

		for(int i = 0; i < numSamples; ++i)
		{
			double frequency = 440.0;
			if(frequencyMod)
			{
				frequency = frequencyMod->NextSample();
			}

			double amplitude = 1.0;
			if(amplitudeEnv)
			{
				amplitude = amplitudeEnv->NextSample();
			}

			double sample = 0.0;
			for(int j = 0; j < numOscilltors; ++j)
			{
				sample += oscillators[j].NextSample(frequency * frequencies[j]) * amplitudes[j];
			}

			buffer[i] = sample * amplitude * 32767;
		}

		if(frequencyMod)
		{
			frequencyMod->ResetStream();
		}

		if(amplitudeEnv)
		{
			amplitudeEnv->ResetStream();
		}

		file.Append(buffer, numSamples);
	}
}