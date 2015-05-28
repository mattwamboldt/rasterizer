#ifndef AUDIO_FILE_WAVE_H
#define AUDIO_FILE_WAVE_H

#include "../audio.h"
#include "breakpoint.h"
#include "waveheader.h"
#include <vector>
using namespace std;

namespace Audio
{
	class WaveFile
	{
	public:
		WaveFile();

		void Init(const WaveHeader& h);
		
		bool Read(const char* path);
		void Append(PCM16* data, int count);

		bool Write(const char* path);
		void Write(PCM16* data, int count);

		void Play(bool loop = false){ playHead = 0.0; paused = false; looping = loop; }
		bool IsPlaying(){ return !paused && playHead < (header.numSamples / header.numChannels);}

		float GetVolume() const { return volume; }
		void SetVolume(float value);

		float GetPitch() const { return pitch; }
		void SetPitch(float value);

		float GetPan() const { return pan; }
		void SetPan(float value);

		//These are more generic dsp functions but this is the only place I have to test them at the moment
		void Normalize(double decibels);
		PCM16 Peak(Uint32 count, Uint32 offset = 0);

		bool WriteEnvelope(const char* path, Uint32 sampleRateMS = 15); //gives about 66 points per second in the resulting envelope

		bool paused;
		bool looping;

		BreakpointFile* panFile;
	private:
		double playHead;
		double volume;
		double leftGain;
		double rightGain;
		double pan;
		double pitch;

		vector<PCM16> data;

		//Wav header stuff
		WaveHeader header;
	};
}

#endif