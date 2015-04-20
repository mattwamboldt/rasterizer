#include "oscillator.h"
#include <math.h>
#include "../audio.h"
#include "../../debug.h"

#define PI     3.14159265359
#define TWO_PI 6.28318530718

namespace Audio
{
	const char* midiNotes[] = {
		"C  0","C# 0","D  0","D# 0","E  0","F  0","F# 0","G  0","G# 0","A  0","A# 0","B  0",
		"C  1","C# 1","D  1","D# 1","E  1","F  1","F# 1","G  1","G# 1","A  1","A# 1","B  1",
		"C  2","C# 2","D  2","D# 2","E  2","F  2","F# 2","G  2","G# 2","A  2","A# 2","B  2",
		"C  3","C# 3","D  3","D# 3","E  3","F  3","F# 3","G  3","G# 3","A  3","A# 3","B  3",
		"C  4","C# 4","D  4","D# 4","E  4","F  4","F# 4","G  4","G# 4","A  4","A# 4","B  4",
		"C  5","C# 5","D  5","D# 5","E  5","F  5","F# 5","G  5","G# 5","A  5","A# 5","B  5",
		"C  6","C# 6","D  6","D# 6","E  6","F  6","F# 6","G  6","G# 6","A  6","A# 6","B  6",
		"C  7","C# 7","D  7","D# 7","E  7","F  7","F# 7","G  7","G# 7","A  7","A# 7","B  7",
		"C  8","C# 8","D  8","D# 8","E  8","F  8","F# 8","G  8","G# 8","A  8","A# 8","B  8",
		"C  9","C# 9","D  9","D# 9","E  9","F  9","F# 9","G  9","G# 9","A  9","A# 9","B  9",
		"C 10","C# 0","D 10","D#10","E 10","F 10","F#10","G 10","G#10"
	};

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
