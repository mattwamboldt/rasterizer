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
	MidiController midiController;

	void FillAudio(void *userData, Uint8 *audioData, int length)
	{
		// Clear our audio buffer to silence.
		memset(audioData, audioSpec.silence, length);
        midiController.Write((PCM16*)audioData, length / 2);
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
		if (audioSpec.format != AUDIO_S16)
		{
			return false;
		}

		Oscillator::SetSamplingRate(audioSpec.freq);
		WaveTable::SetSamplingRate(audioSpec.freq);

		//We use this wave file for testing purposes
		WaveHeader header;
		header.bitsPerSample = 16;
		header.blockAlign = 2;
		header.format = 1;
		header.nAvgBytesPerSecond = audioSpec.format * 2;
		header.numChannels = 1;
		header.numSamplesPerSecond = audioSpec.format;
		WaveFile tester;
		tester.Init(header);

		{
			PerfTimer test("wabank generation");
			WaveBank waveBank;
			waveBank.AddWave(TRIANGLE_WAVE);
			waveBank.SetTremolo(10.0f);
			waveBank.SetDuration(5.0);
			waveBank.Write(tester);

			tester.Write("test.wav");
		}

		SDL_PauseAudio(0);
		return true;
	}
}
