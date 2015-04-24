#include "oscillator.h"
#include "signalgenerator.h"

#define TREMOLO_DEPTH 0.3

namespace Audio
{
	const double tremoloSourceFactor = 1.0 - TREMOLO_DEPTH;
	const double tremoloDepthFactor = TREMOLO_DEPTH * 0.5;

	void SignalGenerator::WriteTone(char* path, WaveType type)
	{
		SDL_RWops* testfile = SDL_RWFromFile(path, "w");
		if(testfile)
		{
			Oscillator osc;
			osc.SetWaveType(type);

			//Tremolo is an amplitude modulation effect
			Oscillator tremolo;
			tremolo.SetFrequency(tremoloFrequency);

			Uint32 numSamples = samplingRate * duration;
			PCM16* buffer = new PCM16[numSamples];
			memset(buffer, 0, numSamples * 2);

			for(int i = 0; i < numSamples; ++i)
			{
				double frequency = osc.GetFrequency();
				if(frequencyMod)
				{
					frequency = frequencyMod->NextSample();
				}

				double amplitude = 1.0;
				if(amplitudeEnv)
				{
					amplitude = amplitudeEnv->NextSample();
				}

				if(tremoloFrequency > 0.0)
				{
					//Oscillator is -1 to 1 so we make it 0 to 2 the scale to a percentage
					//which is then cut out of a full scale source using the depth value
					//Tremolo formula = (1 - DEPTH) + ((tremolo + 1) / 2) * DEPTH
					//Eliminating/precomputing constant factors gives the below
					amplitude *= tremoloSourceFactor + (tremolo.NextSample() + 1.0) * tremoloDepthFactor;
				}

				if(pulseWidthMod)
				{
					osc.SetPulseWidth(pulseWidthMod->NextSample());
				}

				double sample = osc.NextSample(frequency) * amplitude;

				buffer[i] = sample * 32767;
			}

			if(frequencyMod)
			{
				frequencyMod->ResetStream();
			}

			if(amplitudeEnv)
			{
				amplitudeEnv->ResetStream();
			}

			SDL_RWwrite(testfile, buffer, 2, numSamples);
			SDL_RWclose(testfile);
		}
	}
}