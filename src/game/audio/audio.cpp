#include "audio.h"
#include <SDL/SDL.h>
#include "../debug.h"
#include <cstring>
#include <random>
#include "file/wave.h"
#include "effects/delay.h"
#include "synth/oscillator.h"

namespace Audio
{
	SDL_AudioSpec audioSpec;
	Oscillator test;
	WaveFile wave;
	Delay* delay;

	void FillAudio(void *userData, Uint8 *audioData, int length)
	{
		// Clear our audio buffer to silence.
		memset(audioData, audioSpec.silence, length);
		PCM16* pcmData = (PCM16*)audioData;
		int samplecount = length / 2;
		wave.Write(pcmData, samplecount);
		delay->Write(pcmData, samplecount);
	}

	bool Init()
	{
		//Set our initial properties
		audioSpec.freq = 44100;
		audioSpec.format = AUDIO_S16;
		audioSpec.channels = 2;
		audioSpec.samples = 4400;
		audioSpec.callback = &FillAudio;

		SDL_OpenAudio(&audioSpec, 0);

		//If the audio device gives the expected format then unpause the device
		if(audioSpec.format != AUDIO_S16)
		{
			return false;
		}

		delay = new Delay(1.0f, 0.5f);
		test.SetFrequency(440);
		wave.Load("data/foxworthy1.wav");
		SDL_RWops* outbuffer = SDL_RWFromFile("bird.raw", "w");
		if(outbuffer)
		{
			Uint8 output[4410];
			while(wave.IsPlaying())
			{
				FillAudio(0, output, 4410);
				SDL_RWwrite(outbuffer, output, 1, 4410);
			}

			wave.Play(true);
			SDL_RWclose(outbuffer);
		}
		else
		{
			Debug::console("Failed to open cat.raw\n");
		}

		SDL_PauseAudio(0);
		return true;
	}
}
