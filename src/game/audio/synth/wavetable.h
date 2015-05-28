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

		void Normalize();

		//lets us handle custom write functions
		//This may be better handled by using function pointers to generate our samples
		//but that'll get figured out in the long term and this is more flexible
		double& operator[](unsigned long index){ return table[index]; }

		static void SetSamplingRate(double sr);

		void SetVolume(double value);

	private:
		static double sampleIncrement;

		double volume;

		double phase;
		double increment;
		double frequency;

		double table[TABLELENGTH + 1];
	};
}

#endif
