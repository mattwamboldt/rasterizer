#include "audio.h"
#include <SDL/SDL.h>
#include "../debug.h"
#include "../perftimer.h"
#include <cstring>
#include <random>
#include "file/wave.h"
#include "file/midi.h"
#include "effects/delay.h"
#include "synth/midinote.h"
#include "file/breakpoint.h"
#include "synth/oscillator.h"
#include "synth/signalgenerator.h"
#include "synth/oscillatorbank.h"
#include "synth/wavetable.h"
#include "synth/wavebank.h"

#include <time.h>

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

	WaveHeader outputTestHeader;
	WaveFile outputTest;

	void FillAudio(void *userData, Uint8 *audioData, int length)
	{
		// Clear our audio buffer to silence.
		memset(audioData, audioSpec.silence, length);
		PCM16* pcmData = (PCM16*)audioData;
		int samplecount = length / 2;
		//midi.Advance(&midiController);
		wave.Write(pcmData, samplecount);
		if(outbuffer) SDL_RWwrite(outbuffer, audioData, 1, length);
	}

	bool Init()
	{
		outputTestHeader.bitsPerSample = 16;
		outputTestHeader.blockAlign = 2;
		outputTestHeader.format = WAVE_PCM;
		outputTestHeader.numChannels = 1;
		outputTestHeader.numSamplesPerSecond = 44100;
		outputTestHeader.nAvgBytesPerSecond = outputTestHeader.numSamplesPerSecond * 2;
		outputTest.Init(outputTestHeader);

		PerfTimer::Init();
		PerfTimer audio_init("Audio::Init");
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
		WaveTable::SetSamplingRate(audioSpec.freq);

		BreakpointFile arpeggio;
		arpeggio.Load("data/frequency.txt");
		arpeggio.SetSamplingRate(audioSpec.freq);
		arpeggio.ResetStream();

		clock_t endTime, startTime = clock();
		WaveBank test;
		test.AddWave(TRIANGLE_WAVE);
		test.SetDuration(5.0f);
		test.SetFrequencyModulation(&arpeggio);
		test.Write(outputTest);
		endTime = clock();
		Debug::console("TIME: triangle synthesis time %f seconds\n", (endTime - startTime) / (double)CLOCKS_PER_SEC);
		outputTest.Write("test.wav");

		SDL_PauseAudio(0);
		return true;
	}
}
