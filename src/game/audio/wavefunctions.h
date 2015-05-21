#ifndef AUDIO_WAVEFUNCTIONS_H

namespace Audio
{
	double sin_wave(double phase, double pulseWidth);
	double cos_wave(double phase, double pulseWidth);
	double square_wave(double phase, double pulseWidth);
	double downward_sawtooth_wave(double phase, double pulseWidth);
	double upward_sawtooth_wave(double phase, double pulseWidth);
	double triangle_wave(double phase, double pulseWidth);
}
#endif