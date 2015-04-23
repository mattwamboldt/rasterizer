#include "audio.h"
#include <SDL/SDL.h>
#include "../debug.h"
#include <cstring>
#include <random>
#include "file/wave.h"
#include "file/midi.h"
#include "effects/delay.h"
#include "synth/midinote.h"
#include "file/breakpoint.h"
#include "synth/oscillator.h"

namespace Audio
{
	SDL_AudioSpec audioSpec;
	Oscillator test;
	WaveFile wave;
	Delay* delay;
	MidiFile midi;
	MidiController midiController;
	SDL_RWops* outbuffer;
	BreakpointFile breapoints;
	BreakpointFile pantest;

	void FillAudio(void *userData, Uint8 *audioData, int length)
	{
		// Clear our audio buffer to silence.
		memset(audioData, audioSpec.silence, length);
		PCM16* pcmData = (PCM16*)audioData;
		int samplecount = length / 2;
		//midi.Advance(&midiController);
		midiController.Write(pcmData, samplecount);
		if(outbuffer) SDL_RWwrite(outbuffer, audioData, 1, length);
	}

	void WriteTone(char* path)
	{
		SDL_RWops* testfile = SDL_RWFromFile(path, "w");
		if(testfile)
		{
			Oscillator osc;

			//We want to generate 5 seconds of audio for testing
			Uint32 numSamples = audioSpec.freq * 5;
			PCM16* buffer = new PCM16[numSamples];
			memset(buffer, audioSpec.silence, numSamples * 2);

			for(int i = 0; i < numSamples; ++i)
			{
				buffer[i] = osc.NextSample() * 32767;
			}

			SDL_RWwrite(testfile, buffer, 2, numSamples);
			SDL_RWclose(testfile);
		}
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

		Oscillator::SetSamplingRate(audioSpec.freq);

		WriteTone("data/test.raw");

		delay = new Delay(1.0f, 0.5f);
		pantest.Load("data/pan.txt");

		wave.Load("data/foxworthy1.wav");
		wave.panFile = &pantest;
		wave.Play(false);

		midi.Load("data/megaman2.mid");
		midiController.Init(/*&midi*/);
		outbuffer = SDL_RWFromFile("bird.raw", "w");
		SDL_PauseAudio(0);
		return true;
	}
}
