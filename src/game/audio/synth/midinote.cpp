#include "midinote.h"
#include <math.h>
#include "../audio.h"
#include "../../debug.h"

#define PI     3.14159265359
#define TWO_PI 6.28318530718

namespace Audio
{
	double MidiNote::semitoneRatio = 0.0;
	double MidiNote::c0 = 0.0;
	
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

	void MidiNote::SetScale(Uint32 numSemitones, float refFrequency)
	{
		//The range of Midi is from 0 to 127, We assume Equal temperment
		semitoneRatio = pow(2, 1.0/(float)numSemitones);

		//Middle C is 3 above low A, which is half the frequency of our tuning
		double c5 = (refFrequency / 2.0f) * pow(semitoneRatio, 3);
		c0 = c5 * pow(0.5, 5);
	}

	void MidiNote::SetMidiNote(Uint8 note)
	{
		wave.SetFrequency(c0 * pow(semitoneRatio, note));
	}

	void MidiNote::Press(Uint8 velocity)
	{
		if(!playing)
		{
			Debug::console("playing note\n");
			sampleCount = 0;
			wave.Reset();
			currentState = ENV_ATTACK;
			playing = true;
		}
	}

	void MidiNote::Release(Uint8 velocity)
	{
		currentState = ENV_RELEASE;
		sampleCount = 0;
	}

	MidiNote::MidiNote() : volume(0.5), playing(false)
	{
		attack = 0;
		decay = 0;
		release = 0;
	}

	void MidiNote::SetADSR(BreakpointFile* atk, BreakpointFile* dec, BreakpointFile* rel)
	{
		attack = atk;
		decay = dec;
		release = rel;
	}

	void MidiNote::SetWaveType(WaveType value)
	{
		wave.SetWaveType(value);
	}

	float MidiNote::GetEnvelope()
	{
		if(currentState == ENV_ATTACK && attack)
		{
			return attack->At(sampleCount).value;
		}
		else if(currentState == ENV_DECAY && decay)
		{
			return decay->At(sampleCount).value;
		}
		else if(currentState == ENV_SUSTAIN && decay)
		{
			return decay->At(decay->NumPoints() - 1).value;
		}
		else if(currentState == ENV_RELEASE && release)
		{
			return release->At(sampleCount).value;
		}

		return 1.0f;
	}

	void MidiNote::Write(PCM16* data, int count)
	{
		if(!playing || volume < 0.00001) return;

		for(int i = 0; i < count; i +=2)
		{
			float envelope = GetEnvelope();
			double value = wave.NextSample(wave.GetFrequency()) * volume * envelope;
			double channelOne = data[i] / 32767.0;
			double channelTwo = data[i + 1] / 32767.0;
			data[i] = (value + channelOne - value * channelOne) * 32767.0;
			data[i+1] = (value + channelTwo - value * channelTwo) * 32767.0;

			//Update the state of the envelope
			++sampleCount;
			if(currentState == ENV_ATTACK)
			{
				if(attack && sampleCount >= attack->NumPoints())
				{
					currentState = ENV_DECAY;
					sampleCount = 0;
				}
			}
			else if(currentState == ENV_DECAY)
			{
				if(decay && sampleCount >= decay->NumPoints())
				{
					currentState = ENV_SUSTAIN;
					sampleCount = 0;
				}
			}
			else if(currentState == ENV_RELEASE)
			{
				if(release)
				{
					playing = sampleCount >= release->NumPoints();
				}
				else
				{
					playing = false;
				}
			}
		}
	}
}
