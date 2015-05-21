#ifndef AUDIO_SYNTH_WAVEBANK_H
#define AUDIO_SYNTH_WAVEBANK_H

#include "..\types.h"
#include "wavetable.h"
#include <vector>
using namespace std;

namespace Audio
{
	class WaveBank
	{
	public:
		void AddWave(WaveType waveType);
	private:
		vector<WaveTable> waves;
	};
}

#endif