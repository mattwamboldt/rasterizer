#include "wave.h"
#include "../../debug.h"
#include <cstring>

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

	WaveFile::WaveFile()
		:playHead(0.0), volume(0.5), data(0), paused(true), looping(false), pitch(1.0f)
	{}

	WaveFile::~WaveFile()
	{
		if(data)
		{
			delete [] data;
			data = 0;
		}
	}

	bool WaveFile::Load(const char* path)
	{
		if(data)
		{
			delete [] data;
			data = 0;
		}

		playHead = 0.0;
		paused = true;
		looping = false;
		volume = 1.0f;
		pitch = 1.0f;

		SetPan(0.0f);

		SDL_RWops* file = SDL_RWFromFile( path, "r+b" );
		//File does not exist
		if( file == NULL )
		{
			Debug::console( "Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );
	        return false;
		}
		else
		{
			//WAV files are a little endian format so we can't do any fancy memory mapping stuff.
			//TODO: Write a tool that converts source audio files to a proprietary base raw format for faster reads
			char chunkID[4];

			SDL_RWread( file, chunkID, 4, 1);
			if(strncmp(chunkID, "RIFF", 4))
			{
				Debug::console("WAV PARSER: invalid header format\n");
				return false;
			}

			//We ignore the size here
			Uint32 chunkSize = SDL_ReadLE32( file );

			SDL_RWread( file, chunkID, 4, 1);
			if(strncmp(chunkID, "WAVE", 4))
			{
				Debug::console("WAV PARSER: invalid header format\n");
				return false;
			}

			SDL_RWread( file, chunkID, 4, 1);
			if(strncmp(chunkID, "fmt ", 4))
			{
				Debug::console("WAV PARSER: invalid header format\n");
				return false;
			}

			//Should be 16, 18 or 40 depending on the file contents
			//but we only care about the first 16 bytes for pcm support
			chunkSize = SDL_ReadLE32( file );

			format = SDL_ReadLE16(file);
			numChannels = SDL_ReadLE16(file);

			numSamplesPerSecond = SDL_ReadLE32(file);
			nAvgBytesPerSecond = SDL_ReadLE32(file);

			blockAlign = SDL_ReadLE16(file);
			bitsPerSample = SDL_ReadLE16(file);

			if(format != WAVE_PCM)
			{
				Debug::console("WAV PARSER: file not PCM\n");
				return false;
			}

			SDL_RWseek(file, chunkSize - 16, RW_SEEK_CUR);

			SDL_RWread( file, chunkID, 4, 1);
			if(strncmp(chunkID, "data ", 4))
			{
				Debug::console("WAV PARSER: Excess chunks that we can't deal with yet\n");
				return false;
			}

			chunkSize = SDL_ReadLE32( file );

			Uint8 bytesPerSample = bitsPerSample / 8;
			numSamples = chunkSize / bytesPerSample;
			data = new PCM16[numSamples];

			if(bytesPerSample == 1)
			{
				for(int i = 0; i < numSamples; ++i)
				{
					data[i] = (((Uint16)SDL_ReadU8(file)) - 128) << 8;
				}
			}
			else if(bytesPerSample == 2)
			{
				for(int i = 0; i < numSamples; ++i)
				{
					data[i] = SDL_ReadLE16(file);
				}
			}
			else
			{
				Debug::console("WAV PARSER: sample size not 8 or 16 bit, we can't deal yet.\n");
				return false;
			}

			SDL_RWclose( file );
			return true;
		}
	}

	double lerp(double v0, double v1, double t)
	{
		return ((1-t)* v0) + (t * v1);
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

		// Linear panning
		float position = pan * 0.5f;
		leftGain = 0.5f - position;
		rightGain = 0.5f + position;
	}

	void WaveFile::Write(PCM16* buffer, int count)
	{
		if(data && !paused)
		{
			int i = 0;

			//This is how many samples to increment before playing the next sample
			double playheadIncrement =  (numSamplesPerSecond / (double)audioSpec.freq) * pitch;

			while(playHead < (numSamples / numChannels) && i < count)
			{
				//The playhead will be in relation to a mono track and scaled
				Uint32 playHeadsample = ((Uint32)playHead) * numChannels;
				double t = 0.0;
				Uint32 nextSample = playHeadsample;
				
				//We do a lerp to scale the audio
				if(playHeadsample < (numSamples / numChannels) - 1)
				{
					t = playHead - playHeadsample;
					nextSample = playHeadsample + numChannels; 
				}

				float leftChannel = lerp(data[playHeadsample], data[nextSample], t);
				float rightChannel = leftChannel; //For mono we simply output the same sample to both channels

				if(numChannels == 2)
				{
					rightChannel = lerp(data[playHeadsample+1], data[nextSample+1], t);
				}

				buffer[i] = (PCM16)(leftChannel * volume * leftGain);
				buffer[i+1] = (PCM16)(rightChannel * volume * rightGain);

				i += 2;
				playHead += playheadIncrement;

				if(playHead >= (numSamples / numChannels) && looping)
				{
					playHead = 0.0;
				}
			}
		}
	}
}
