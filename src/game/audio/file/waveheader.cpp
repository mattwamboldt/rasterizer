#include "waveheader.h"
#include "..\..\debug.h"

namespace Audio
{
	bool WaveHeader::Read(SDL_RWops* file)
	{
		//WAV files are a little endian format so we can't do any fancy memory mapping stuff.
		char chunkID[4];

		SDL_RWread(file, chunkID, 4, 1);
		if (strncmp(chunkID, "RIFF", 4))
		{
			Debug::console("WAV PARSER: invalid header format\n");
			return false;
		}

		//We ignore the size here
		Uint32 chunkSize = SDL_ReadLE32(file);

		SDL_RWread(file, chunkID, 4, 1);
		if (strncmp(chunkID, "WAVE", 4))
		{
			Debug::console("WAV PARSER: invalid header format\n");
			return false;
		}

		SDL_RWread(file, chunkID, 4, 1);
		if (strncmp(chunkID, "fmt ", 4))
		{
			Debug::console("WAV PARSER: invalid header format\n");
			return false;
		}

		//Should be 16, 18 or 40 depending on the file contents
		//but we only care about the first 16 bytes for pcm support
		chunkSize = SDL_ReadLE32(file);

		format = SDL_ReadLE16(file);
		numChannels = SDL_ReadLE16(file);

		numSamplesPerSecond = SDL_ReadLE32(file);
		nAvgBytesPerSecond = SDL_ReadLE32(file);

		blockAlign = SDL_ReadLE16(file);
		bitsPerSample = SDL_ReadLE16(file);

		if (format != WAVE_PCM)
		{
			Debug::console("WAV PARSER: file not PCM\n");
			return false;
		}

		SDL_RWseek(file, chunkSize - 16, RW_SEEK_CUR);
		SDL_RWread(file, chunkID, 4, 1);
		if (strncmp(chunkID, "data ", 4))
		{
			Debug::console("WAV PARSER: Excess chunks that we can't deal with yet\n");
			return false;
		}

		chunkSize = SDL_ReadLE32(file);

		Uint8 bytesPerSample = bitsPerSample / 8;
		numSamples = chunkSize / bytesPerSample;
		return true;
	}

	void WaveHeader::Write(SDL_RWops* file)
	{
		Uint32 headerSize = 16;
		Uint32 dataSize = numSamples * sizeof(PCM16);

		SDL_RWwrite(file, "RIFF", 4, 1);

		//We ignore the size here
		SDL_WriteLE32(file, headerSize + dataSize + 20);

		SDL_RWwrite(file, "WAVE", 4, 1);
		SDL_RWwrite(file, "fmt ", 4, 1);

		//We only care about the first 16 bytes for pcm support
		SDL_WriteLE32(file, headerSize);

		SDL_WriteLE16(file, format);
		SDL_WriteLE16(file, numChannels);
		SDL_WriteLE32(file, numSamplesPerSecond);
		SDL_WriteLE32(file, nAvgBytesPerSecond);
		SDL_WriteLE16(file, blockAlign);
		SDL_WriteLE16(file, bitsPerSample);

		SDL_RWwrite(file, "data", 4, 1);
		SDL_WriteLE32(file, dataSize);
	}
}