#include "Spectrum.h"

bool SpectrumSamplesSorted(const Real* _lambda, const Real* _vals, const unsigned _n) {
	for (unsigned i = 0; i < _n - 1; ++i)
		if (_lambda[i] > _lambda[i + 1]) return false;
	return true;
}

void  SortSpectrumSamples(Real* _lambda, Real* _vals, const unsigned _n) {
	std::vector<std::pair<Real, Real>> sortVec;
	sortVec.reserve(_n);
	for (unsigned i = 0; i < _n; ++i)
		sortVec.push_back(std::make_pair(_lambda[i], _vals[i]));
	std::sort(sortVec.begin(), sortVec.end());
	for (unsigned i = 0; i < _n; ++i) {
		_lambda[i] = sortVec[i].first;
		_vals[i] = sortVec[i].second;
	}
}

Real AverageSpectrumSamples(const Real* _lambda, const Real* _vals, const unsigned _n, const Real _lambdaStart, const Real _lambdaEnd) {
	// Handle cases with out-of-bounds range or single sample only
	if (_n == 1 || _lambdaEnd <= _lambda[0]) return _vals[0];
	if (_lambdaStart >= _lambda[_n - 1]) return _vals[_n - 1];

	// Add contributions of constant (rectangular) segments before/after samples
	Real sum = 0.;
	if (_lambdaStart < _lambda[0]) sum += _vals[0] * (_lambda[0] - _lambdaStart);
	if (_lambdaEnd > _lambda[_n - 1]) sum += _vals[_n - 1] * (_lambdaEnd - _lambda[_n - 1]);

	// Advance to first relevant wavelength segment
	unsigned i = 0;
	while (_lambda[i + 1] < _lambdaStart) ++i;

	// Interpolate vals between first (i) and second (i+1) lambda and add that segment
	const Real vs = maths::Lerp(_vals[i], _vals[i + 1], (_lambdaStart - _lambda[i]) / (_lambda[i + 1] - _lambda[i]));
	sum += .5 * (_lambda[i + 1] - _lambdaStart) * (vs + _vals[i + 1]);
	++i;
	// Add contributions of middle segments that are not touching start/end boundary
	for (; i < _n - 1 && _lambdaEnd >= _lambda[i]; ++i)
		sum += .5 * (_lambda[i + 1] - _lambda[i]) * (_vals[i] + _vals[i + 1]);

	// Add end segment using end interpolated value
	const Real ve = maths::Lerp(_vals[i], _vals[i + 1], (_lambdaEnd - _lambda[i]) / (_lambda[i + 1] - _lambda[i]));
	sum += .5 * (_lambda[i + 1] - _lambda[i]) * (ve + _vals[i]);

	return sum / (_lambdaEnd - _lambdaStart);
}

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

RGBSpectrum SampledSpectrum::ToRGBSpectrum() const {
	Real rgb[3];
	ToRGB(rgb);
	return RGBSpectrum::FromRGB(rgb);
}

SampledSpectrum::SampledSpectrum(const RGBSpectrum& _r, SpectrumType _type) {
	Real rgb[3];
	_r.ToRGB(rgb);
	*this = SampledSpectrum::FromRGB(rgb, _type);
}

SampledSpectrum SampledSpectrum::X;
SampledSpectrum SampledSpectrum::Y;
SampledSpectrum SampledSpectrum::Z;
SampledSpectrum SampledSpectrum::rgbRefl2SpectWhite;
SampledSpectrum SampledSpectrum::rgbRefl2SpectCyan;
SampledSpectrum SampledSpectrum::rgbRefl2SpectMagenta;
SampledSpectrum SampledSpectrum::rgbRefl2SpectYellow;
SampledSpectrum SampledSpectrum::rgbRefl2SpectRed;
SampledSpectrum SampledSpectrum::rgbRefl2SpectGreen;
SampledSpectrum SampledSpectrum::rgbRefl2SpectBlue;
SampledSpectrum SampledSpectrum::rgbIllum2SpectWhite;
SampledSpectrum SampledSpectrum::rgbIllum2SpectCyan;
SampledSpectrum SampledSpectrum::rgbIllum2SpectMagenta;
SampledSpectrum SampledSpectrum::rgbIllum2SpectYellow;
SampledSpectrum SampledSpectrum::rgbIllum2SpectRed;
SampledSpectrum SampledSpectrum::rgbIllum2SpectGreen;
SampledSpectrum SampledSpectrum::rgbIllum2SpectBlue;