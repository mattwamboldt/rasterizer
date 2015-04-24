#ifndef AUDIO_SYNTH_WAVETABLE
#define AUDIO_SYNTH_WAVETABLE

#define TABLELENGTH (1024)

namespace Audio
{
	class WaveTable
	{
	public:
		WaveTable();

		void Reset(double startphase = 0.0){ phase = TABLELENGTH * startphase; }
		double NextSample();
		double NextSample(double freq);

		void SetFrequency(double frequency);
		double GetFrequency(){ return frequency; };
		static void SetSamplingRate(double sr);

	private:
		static double sampleIncrement;

		double phase;
		double increment;
		double frequency;

		double table[TABLELENGTH];
	};
}

#endif
