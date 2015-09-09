#ifndef AUDIO_EFFECTS_IAUDIOEFFECT_H
#define AUDIO_EFFECTS_IAUDIOEFFECT_H
#include "..\types.h"

namespace Audio
{
	class IAudioEffect
	{
	public:
		virtual void Write(PCM16* data, int count) = 0;
	};
}

#endif