#include "delay.h"

namespace Audio
{
	Delay::Delay(float time, float decay)
		: position(0), decay(decay)
	{
		size = (int)(time * audioSpec.freq);
		buffer = new PCM16[size];
		memset(buffer, 0, size * 2);
	}

	Delay::~Delay()
	{
		if(buffer)
		{
			delete[] buffer;
			buffer = 0;
		}
	}

	void Delay::Write(PCM16* data, int count)
	{
		for(int i = 0; i < count; ++i)
		{
			data[i] = (PCM16)(data[i] + buffer[position] * decay);
			buffer[position] = data[i];
			++position;
			if(position >= size)
			{
				position = 0;
			}
		}
	}
}
