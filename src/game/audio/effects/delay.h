#ifndef AUDIO_EFFECTS_DELAY
#define AUDIO_EFFECTS_DELAY

#include "../audio.h"

namespace Audio
{
	class Delay
	{
	public:
		Delay(float time, float decay);
		~Delay();
		void Write(PCM16* data, int count);

	private:
		PCM16* buffer;
		int size;
		int position;
		float decay;
	};
}

#endif
