//----	By Sam Warren 2019	----

#include "Spectrum.h"

SampledSpectrum SampledSpectrum::FromRGB(const Real _rgb[3], const SpectrumType _type) {
	SampledSpectrum r;
	if (_type == SpectrumType::Reflectance) {
		// Convert reflectance spectrum to RGB
		if (_rgb[0] <= _rgb[1] && _rgb[0] <= _rgb[2]) {
			// Compute reflectance _SampledSpectrum_ with __rgb[0]_ as minimum
			r += rgbRefl2SpectWhite * _rgb[0];
			if (_rgb[1] <= _rgb[2]) {
				r += rgbRefl2SpectCyan * (_rgb[1] - _rgb[0]);
				r += rgbRefl2SpectBlue * (_rgb[2] - _rgb[1]);
			}
			else {
				r += rgbRefl2SpectCyan * (_rgb[2] - _rgb[0]);
				r += rgbRefl2SpectGreen * (_rgb[1] - _rgb[2]);
			}
		}
		else if (_rgb[1] <= _rgb[0] && _rgb[1] <= _rgb[2]) {
			// Compute reflectance _SampledSpectrum_ with __rgb[1]_ as minimum
			r += rgbRefl2SpectWhite * _rgb[1];
			if (_rgb[0] <= _rgb[2]) {
				r += rgbRefl2SpectMagenta * (_rgb[0] - _rgb[1]);
				r += rgbRefl2SpectBlue * (_rgb[2] - _rgb[0]);
			}
			else {
				r += rgbRefl2SpectMagenta * (_rgb[2] - _rgb[1]);
				r += rgbRefl2SpectRed * (_rgb[0] - _rgb[2]);
			}
		}
		else {
			// Compute reflectance _SampledSpectrum_ with __rgb[2]_ as minimum
			r += rgbRefl2SpectWhite * _rgb[2];
			if (_rgb[0] <= _rgb[1]) {
				r += rgbRefl2SpectYellow * (_rgb[0] - _rgb[2]);
				r += rgbRefl2SpectGreen * (_rgb[1] - _rgb[0]);
			}
			else {
				r += rgbRefl2SpectYellow * (_rgb[1] - _rgb[2]);
				r += rgbRefl2SpectRed * (_rgb[0] - _rgb[1]);
			}
		}
		r *= .94;
	}
	else {
		// Convert illuminant spectrum to RGB
		if (_rgb[0] <= _rgb[1] && _rgb[0] <= _rgb[2]) {
			// Compute illuminant _SampledSpectrum_ with __rgb[0]_ as minimum
			r += rgbIllum2SpectWhite * _rgb[0];
			if (_rgb[1] <= _rgb[2]) {
				r += rgbIllum2SpectCyan * (_rgb[1] - _rgb[0]);
				r += rgbIllum2SpectBlue * (_rgb[2] - _rgb[1]);
			}
			else {
				r += rgbIllum2SpectCyan * (_rgb[2] - _rgb[0]);
				r += rgbIllum2SpectGreen * (_rgb[1] - _rgb[2]);
			}
		}
		else if (_rgb[1] <= _rgb[0] && _rgb[1] <= _rgb[2]) {
			// Compute illuminant _SampledSpectrum_ with __rgb[1]_ as minimum
			r += rgbIllum2SpectWhite * _rgb[1];
			if (_rgb[0] <= _rgb[2]) {
				r += rgbIllum2SpectMagenta * (_rgb[0] - _rgb[1]);
				r += rgbIllum2SpectBlue * (_rgb[2] - _rgb[0]);
			}
			else {
				r += rgbIllum2SpectMagenta * (_rgb[2] - _rgb[1]);
				r += rgbIllum2SpectRed * (_rgb[0] - _rgb[2]);
			}
		}
		else {
			// Compute illuminant _SampledSpectrum_ with __rgb[2]_ as minimum
			r += rgbIllum2SpectWhite * _rgb[2];
			if (_rgb[0] <= _rgb[1]) {
				r += rgbIllum2SpectYellow * (_rgb[0] - _rgb[2]);
				r += rgbIllum2SpectGreen * (_rgb[1] - _rgb[0]);
			}
			else {
				r += rgbIllum2SpectYellow * (_rgb[1] - _rgb[2]);
				r += rgbIllum2SpectRed * (_rgb[0] - _rgb[1]);
			}
		}
		r *= .86445f;
	}
	return r.Clamp();
}

SampledSpectrum SampledSpectrum::FromXYZ(const Real _xyz[3], const SpectrumType _type = SpectrumType::Reflectance) {
	Real rgb[3];
	SpectrumUtils::XYZToRGB(_xyz, rgb);
	return FromRGB(rgb, _type);
}

SampledSpectrum SampledSpectrum::X;
SampledSpectrum SampledSpectrum::Y;
SampledSpectrum SampledSpectrum::Z;
SampledSpectrum SampledSpectrum::rgbRefl2SpectWhite = NULL;
SampledSpectrum SampledSpectrum::rgbRefl2SpectCyan = NULL;
SampledSpectrum SampledSpectrum::rgbRefl2SpectMagenta = NULL;
SampledSpectrum SampledSpectrum::rgbRefl2SpectYellow = NULL;
SampledSpectrum SampledSpectrum::rgbRefl2SpectRed = NULL;
SampledSpectrum SampledSpectrum::rgbRefl2SpectGreen = NULL;
SampledSpectrum SampledSpectrum::rgbRefl2SpectBlue = NULL;
SampledSpectrum SampledSpectrum::rgbIllum2SpectWhite = NULL;
SampledSpectrum SampledSpectrum::rgbIllum2SpectCyan = NULL;
SampledSpectrum SampledSpectrum::rgbIllum2SpectMagenta = NULL;
SampledSpectrum SampledSpectrum::rgbIllum2SpectYellow = NULL;
SampledSpectrum SampledSpectrum::rgbIllum2SpectRed = NULL;
SampledSpectrum SampledSpectrum::rgbIllum2SpectGreen = NULL;
SampledSpectrum SampledSpectrum::rgbIllum2SpectBlue = NULL;