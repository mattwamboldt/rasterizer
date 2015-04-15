#include "audio.h"
#include <SDL/SDL.h>
#include <math.h>
#include "debug.h"
#include <cstring>

#define PI     3.14159265359
#define TWO_PI 6.28318530718

namespace Audio
{
	SDL_AudioSpec audioSpec;
	Oscillator test;
	WaveFile wave;

	void FillAudio(void *userData, Uint8 *audioData, int length)
	{
		// Clear our audio buffer to silence.
		memset(audioData, audioSpec.silence, length);
		PCM16* pcmData = (PCM16*)audioData;
		int samplecount = length / 2;
		wave.Write(pcmData, samplecount);
	}

	double sine_wave(double phase)
	{
		return sin(phase);
	}

	double square_wave(double phase)
	{
		if(phase <= PI)
		{
			return 1.0;
		}
		else
		{
			return -1.0;
		}
	}

	double downward_sawtooth_wave(double phase)
	{
		return 1.0 - 2.0 * (phase / TWO_PI);
	}

	double upward_sawtooth_wave(double phase)
	{
		return 2.0 * (phase / TWO_PI) - 1.0;
	}

	double triangle_wave(double phase)
	{
		double result = upward_sawtooth_wave(phase);
		if(result < 0.0) result = -result;
		return 2.0 * (result - 0.5);
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

		test.SetFrequency(440);
		wave.Load("data/bird.wav");
		SDL_RWops* outbuffer = SDL_RWFromFile("bird.raw", "w");
		if(outbuffer)
		{
			Uint8 output[4410];
			while(wave.playHead < (wave.numSamples / wave.numChannels))
			{
				FillAudio(0, output, 4410);
				SDL_RWwrite(outbuffer, output, 1, 4410);
			}

			wave.playHead = 0.0;
			SDL_RWclose(outbuffer);
		}
		else
		{
			Debug::console("Failed to open cat.raw\n");
		}

		SDL_PauseAudio(0);
		return true;
	}

	Oscillator::Oscillator() : phase(0.0), volume(0.5)
	{
		SetFrequency(440);
	}

	void Oscillator::SetFrequency(double frequency)
	{
		increment = frequency / audioSpec.freq * TWO_PI;
	}

	void Oscillator::Write(PCM16* data, int count)
	{
		for(int i = 0; i < count; i +=2)
		{
			double value = sine_wave(phase) * 32767.0 * volume;
			data[i] = value;
			data[i+1] = value;

			phase += increment;

			if(phase >= TWO_PI)
			{
				phase -= TWO_PI;
			}
		}
	}

	WaveFile::WaveFile()
		:playHead(0.0), volume(0.5), data(0)
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

	void WaveFile::Write(PCM16* buffer, int count)
	{
		if(data)
		{
			int i = 0;

			//This is how many samples to increment before playing the next sample
			double playheadIncrement =  numSamplesPerSecond / (double)audioSpec.freq;

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

				buffer[i] = lerp(data[playHeadsample], data[nextSample], t);

				//For mono we simply output the same sample to both channels
				if(numChannels == 1)
				{
					buffer[i+1] = buffer[i];
				}
				else
				{
					buffer[i+1] = lerp(data[playHeadsample+1], data[nextSample+1], t);
				}

				i += 2;
				playHead += playheadIncrement;
			}
		}
	}
}