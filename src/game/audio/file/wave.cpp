#include "wave.h"
#include "../../debug.h"
#include "../util.h"
#include <cstring>
#include <math.h>

//TODO: Separate the file reading from the sound itself so that generic functions can be applied to the read buffer

namespace Audio
{
	WaveFile::WaveFile()
		:playHead(0.0), volume(0.5), data(0), paused(true), looping(false), pitch(1.0f), panFile(0)
	{}

	bool WaveFile::Read(const char* path)
	{
		data.clear();
		playHead = 0.0;
		paused = true;
		looping = false;
		volume = 1.0f;
		pitch = 1.0f;

		SetPan(0.0f);

		SDL_RWops* file = SDL_RWFromFile( path, "r+b" );
		
		if( file == NULL )
		{
			Debug::console( "Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );
	        return false;
		}
		
		if (!header.Read(file))
		{
			Debug::console("WAV PARSER: Failed to read header.\n");
			SDL_RWclose(file);
			return false;
		}

		Uint8 bytesPerSample = header.bitsPerSample / 8;
		if (bytesPerSample == 1)
		{
			//We scale 8 bit samples to work in 16
			for (Uint32 i = 0; i < header.numSamples; ++i)
			{
				data.push_back((((Uint16)SDL_ReadU8(file)) - 128) << 8);
			}

			header.blockAlign *= 2;
			header.nAvgBytesPerSecond *= 2;
			bytesPerSample = 2;
			header.bitsPerSample = 16;
		}
		else if (bytesPerSample == 2)
		{
			for (Uint32 i = 0; i < header.numSamples; ++i)
			{
				data.push_back(SDL_ReadLE16(file));
			}
		}
		else if (bytesPerSample == 3)
		{
			Sint32 fullSample;
			for (Uint32 i = 0; i < header.numSamples; ++i)
			{
				fullSample = 0;
				fullSample |= SDL_ReadU8(file) << 8;
				fullSample |= SDL_ReadU8(file) << 16;
				fullSample |= SDL_ReadU8(file) << 24;
				data.push_back((fullSample / 2147483648.0f) * 32767.0f);
			}

			header.blockAlign = 2 * header.numChannels;
			header.nAvgBytesPerSecond = header.numSamplesPerSecond * header.blockAlign;
			bytesPerSample = 2;
			header.bitsPerSample = 16;
		}
		else
		{
			Debug::console("WAV PARSER: sample size not 8, 16 or 24 bit, we can't deal yet.\n");
			SDL_RWclose(file);
			return false;
		}

		SDL_RWclose( file );
		return true;
	}

	void WaveFile::Init(const WaveHeader& h)
	{
		header = h;
	}

	void WaveFile::Append(PCM16* source, int count)
	{
		for (int i = 0; i < count; i++)
		{
			data.push_back(source[i]);
			if (header.numChannels == 2)
			{
				data.push_back(source[i]);
			}
		}
	}

	bool WaveFile::Write(const char* path)
	{
		if(data.size() == 0) return false;
		
		SDL_RWops* file = SDL_RWFromFile( path, "w+b" );
		if( file == NULL )
		{
			Debug::console( "Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );
	        return false;
		}

		header.numSamples = data.size();
		header.Write(file);

		for (Uint32 i = 0; i < header.numSamples; ++i)
		{
			SDL_WriteLE16(file, data[i]);
		}

		SDL_RWclose( file );
		return true;
	}

	bool WaveFile::WriteEnvelope(const char* path, Uint32 sampleRateMS)
	{
		BreakpointFile file;
		Breakpoint newPoint;
		
		float timePerFrame = sampleRateMS / 1000.0f;
		Uint32 samplesPerFrame = timePerFrame * header.numSamplesPerSecond;

		Uint32 sampleOffset = 0;
		float time = 0.0f;

		while (sampleOffset < header.numSamples)
		{
			newPoint.time = time;
			newPoint.value = Peak(samplesPerFrame, sampleOffset) / 32768.0f;
			sampleOffset += samplesPerFrame;
			time += timePerFrame;
			file.Add(newPoint);
		}

		return file.Write(path);
	}

	void WaveFile::SetVolume(float value)
	{
		if(value < 0.0f)
		{
			volume = 0.0f;
		}
		else if(value > 1.0f)
		{
			value = 1.0f;
		}
		else
		{
			volume = value;
		}
	}

	void WaveFile::SetPitch(float value)
	{
		if(value < 0.25f)
		{
			pitch = 0.25f;
		}
		else if(value > 4.0f)
		{
			pitch = 4.0f;
		}
		else
		{
			pitch = value;
		}
	}

	void WaveFile::SetPan(float value)
	{
		if(value < -1.0f)
		{
			pan = -1.0f;
		}
		else if(value > 1.0f)
		{
			pan = 1.0f;
		}
		else
		{
			pan = value;
		}

		// Constant Power Panning
		const double piOver4 = atan(1.0); // each channel is 1/4 of a cycle
		const double root2Over2 = sqrt(2.0) * 0.5;

		// scales the pan to the right range
		double angle = pan * piOver4;
		double cosAngle = cos(angle);
		double sinAngle = sin(angle);
		leftGain = root2Over2 * (cosAngle - sinAngle);
		rightGain = root2Over2 * (cosAngle + sinAngle);

		// Linear panning (for reference)
		// float position = pan * 0.5f;
		// leftGain = 0.5f - position;
		// rightGain = 0.5f + position;
	}

	void WaveFile::Write(PCM16* buffer, int count)
	{
		if(data.size() && !paused)
		{
			int i = 0;

			//This is how many samples to increment before playing the next sample
			double playheadIncrement = (header.numSamplesPerSecond / (double)audioSpec.freq) * pitch;

			while (playHead < (header.numSamples / header.numChannels) && i < count)
			{
				//The playhead will be in relation to a mono track and scaled
				Uint32 playHeadsample = ((Uint32)playHead) * header.numChannels;
				double t = 0.0;
				Uint32 nextSample = playHeadsample;
				
				//We do a lerp to scale the audio
				if (playHeadsample < (header.numSamples / header.numChannels) - 1)
				{
					t = playHead - (playHeadsample / (float)header.numChannels);
					nextSample = playHeadsample + header.numChannels;
				}

				float leftChannel = lerp(data[playHeadsample], data[nextSample], t);
				float rightChannel = leftChannel; //For mono we simply output the same sample to both channels

				if (header.numChannels == 2)
				{
					rightChannel = lerp(data[playHeadsample+1], data[nextSample+1], t);
				}

				if(panFile)
				{
					SetPan(panFile->Value(playHead / header.numSamplesPerSecond));
				}

				buffer[i] = (PCM16)(leftChannel * volume * leftGain);
				buffer[i+1] = (PCM16)(rightChannel * volume * rightGain);

				i += 2;
				playHead += playheadIncrement;

				if (playHead >= (header.numSamples / header.numChannels) && looping)
				{
					playHead = 0.0;
				}
			}
		}
	}

	PCM16 WaveFile::Peak(Uint32 count, Uint32 offset)
	{
		PCM16 peak = 0;
		PCM16 absval = 0;
		for (Uint32 i = 0; i < count && i + offset < header.numSamples; ++i)
		{
			absval = abs(data[i + offset]);
			if(absval > peak)
			{
				peak = absval;
			}
		}

		return peak;
	}

	void WaveFile::Normalize(double decibels)
	{
		//We don't do anything above peak
		if(decibels > 0.0) return;

		//converts from db to amplitude
		float maxAmplitude = pow(10.0, decibels/20.0);

		PCM16 peak = Peak(header.numSamples);

		//We don't process silent files
		if(peak == 0) return;

		float scale = maxAmplitude / (peak / 32768.0f);
		for (Uint32 i = 0; i < header.numSamples; ++i)
		{
			data[i] *= scale;
		}
	}
}
