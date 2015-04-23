#ifndef AUDIO_FILE_WAVE_H
#define AUDIO_FILE_WAVE_H

#include "../audio.h"
#include "breakpoint.h"

namespace Audio
{
	class WaveFile
	{
	public:
		WaveFile();
		~WaveFile();
		
		bool Load(const char* path);
		bool Write(const char* path);
		bool WriteEnvelope(const char* path, Uint32 sampleRateMS = 15); //gives about 66 points per second in the resulting envelope

		void Play(bool loop = false){ playHead = 0.0; paused = false; looping = loop; }
		void Write(PCM16* data, int count);
		bool IsPlaying(){ return !paused && playHead < (numSamples / numChannels);}

		float GetVolume() const { return volume; }
		void SetVolume(float value);

		float GetPitch() const { return pitch; }
		void SetPitch(float value);

		float GetPan() const { return pan; }
		void SetPan(float value);

		//These are more generic dsp functions but this is the only place I have to test them at the moment
		void Normalize(double decibels);
		PCM16 Peak(Uint32 count, Uint32 offset = 0);

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

		PCM16* data;

		//Wav header stuff
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