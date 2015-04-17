#include "oscillator.h"
#include <math.h>
#include "../audio.h"

#define PI     3.14159265359
#define TWO_PI 6.28318530718

namespace Audio
{
	void Oscillator::SetMidiNote(Uint8 note)
	{
		//The range of Midi is from 0 to 127
		//For this calculation we are assuming Equal Temperment tuning with concert A at 440Hz

		//12 semitones per octave
		double semitoneRatio = pow(2, 1/12.0);
		//Middle C is 3 above low A, which is half the frequency of our tuning
		double c5 = 220.0 * pow(semitoneRatio, 3);
		double c0 = c5 * pow(0.5, 5);
		SetFrequency(c0 * pow(semitoneRatio, note));
	}

	void Oscillator::Play(Uint8 velocity)
	{
		if(!playing)
		{
			playing = true;
			phase = 0.0;
		}
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

	Oscillator::Oscillator() : phase(0.0), volume(0.5), playing(false)
	{
		SetFrequency(440);
	}

	void Oscillator::SetFrequency(double frequency)
	{
		increment = frequency / audioSpec.freq * TWO_PI;
	}

	void Oscillator::Write(PCM16* data, int count)
	{
		if(!playing || volume <= 0.0001) return;

		for(int i = 0; i < count; i +=2)
		{
			double value = sine_wave(phase) * volume * 32767.0;
			double channelOne = data[i] / 32767.0;
			double channelTwo = data[i + 1] / 32767.0;
			data[i] = (PCM16)(value + channelOne - value * channelOne);
			data[i+1] = (PCM16)(value + channelTwo - value * channelTwo);

			phase += increment;

			if(phase >= TWO_PI)
			{
				phase -= TWO_PI;
			}
		}
	}
}
