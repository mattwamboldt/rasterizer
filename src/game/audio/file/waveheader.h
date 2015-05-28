#ifndef AUDIO_FILE_WAVEHEADER_H
#define AUDIO_FILE_WAVEHEADER_H
#include "..\types.h"
#include <fstream>

namespace Audio
{
	enum WaveFormat
	{
		WAVE_PCM = 1,
		WAVE_FLOAT = 3,
		WAVE_ALAW = 6,
		WAVE_MULAW = 7,
		EXTENSIBLE = 0xFFFE
	};

	struct WaveHeader
	{
		bool Read(SDL_RWops* file);
		void Write(SDL_RWops* file);

		Uint16 format;
		Uint16 numChannels;
		Uint32 numSamplesPerSecond;
		Uint32 nAvgBytesPerSecond;
		Uint16 blockAlign;
		Uint16 bitsPerSample; //We need to scale our data if it's in another format
		Uint32 numSamples;
	};
}

#endif