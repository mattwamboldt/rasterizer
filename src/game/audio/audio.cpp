#include "audio.h"
#include <SDL/SDL.h>
#include "../debug.h"
#include <cstring>
#include <random>
#include "file/wave.h"
#include "file/midi.h"
#include "effects/delay.h"
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

	void FillAudio(void *userData, Uint8 *audioData, int length)
	{
		// Clear our audio buffer to silence.
		memset(audioData, audioSpec.silence, length);
		PCM16* pcmData = (PCM16*)audioData;
		int samplecount = length / 2;
		midi.Advance(&midiController);
		midiController.Write(pcmData, samplecount);
		//test.Write(pcmData, samplecount);
		if(outbuffer) SDL_RWwrite(outbuffer, audioData, 1, length);
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
		test.SetVolume(1.0);
		wave.Load("data/foxworthy1.wav");
		midi.Load("data/megaman2.mid");
		midiController.Init(&midi);
		outbuffer = SDL_RWFromFile("bird.raw", "w");
		SDL_PauseAudio(0);
		return true;
	}
}
