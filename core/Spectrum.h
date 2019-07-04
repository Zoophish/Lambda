//----	By Sam Warren 2019	----
//----	Spectrum / spectral-power-distribution base and children classes; implemented from the PBRT book: https://www.pbrt.org/	----

#pragma once
#include <vector>
#include <maths/maths.h>

static const unsigned sampledLambdaStart = 400;
static const unsigned sampledLambdaEnd = 700;
static const unsigned nSpectralSamples = 60;

template<unsigned spectrumSamples>
class CoefficientSpectrum {
	public:
		static const unsigned nSamples = spectrumSamples;

		CoefficientSpectrum(const Real _v = 0) {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] = _v;
		}

		CoefficientSpectrum &operator+=(const CoefficientSpectrum &_s) {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] += _s[i];
			return *this;
		}
		CoefficientSpectrum &operator-=(const CoefficientSpectrum &_s) {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] -= _s[i];
			return *this;
		}
		CoefficientSpectrum &operator*=(const CoefficientSpectrum &_s) {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] *= _s[i];
			return *this;
		}
		CoefficientSpectrum &operator/=(const CoefficientSpectrum &_s) {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] /= _s[i];
			return *this;
		}

		CoefficientSpectrum operator+(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp = *this;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] += _s[i];
			return tmp;
		}
		CoefficientSpectrum operator-(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp = *this;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] -= _s[i];
			return tmp;
		}
		CoefficientSpectrum operator*(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp = *this;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] *= _s[i];
			return tmp;
		}
		CoefficientSpectrum operator/(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp = *this;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] /= _s[i];
			return tmp;
		}
		CoefficientSpectrum operator+(const Real _s) const {
			CoefficientSpectrum tmp = *this;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] += _s;
			return tmp;
		}
		CoefficientSpectrum operator-(const Real _s) const {
			CoefficientSpectrum tmp = *this;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] -= _s;
			return tmp;
		}
		CoefficientSpectrum operator*(const Real _s) const {
			CoefficientSpectrum tmp = *this;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] *= _s;
			return tmp;
		}
		CoefficientSpectrum operator/(const Real _s) const {
			const Real inv = 1. / _s;
			CoefficientSpectrum tmp = *this;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] *= inv;
			return tmp;
		}
		inline Real &operator[](const unsigned _i) {
			return c[_i];
		}

		bool IsBlack() const {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				if (c[i] != 0) return false;
			return true;
		}
		bool HasNaNs() const {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				if (std::isnan(c[i])) return true;
			return false;
		}

		static friend CoefficientSpectrum Sqrt(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp.c[i] = std::sqrt(_s.c[i]);
			return tmp;
		}
		static friend CoefficientSpectrum Pow(const CoefficientSpectrum &_s, const Real _p) const {
			CoefficientSpectrum tmp;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp.c[i] = std::pow(_s.c[i], _p);
			return tmp;
		}
		static friend CoefficientSpectrum Exp(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp.c[i] = std::exp(_s.c[i]);
			return tmp;
		}

		CoefficientSpectrum Clamp(const Real _min = 0., const Real _max = INFINITY) const {
			CoefficientSpectrum tmp;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp.c[i] = maths::Clamp(c[i], _min, _max);
			return tmp;
		}

	protected:
		Real c[spectrumSamples];
};

static bool SpectrumSamplesSorted(const Real *_lambda, const Real *_vals, const unsigned _n) {
	for (unsigned i = 0; i < _n - 1; ++i)
		if (_lambda[i] > _lambda[i + 1]) return false;
	return true;
}

static void SortSpectrumSamples(Real *_lambda, Real *_vals, const unsigned _n) {
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

static Real AverageSpectrumSamples(const Real* _lambda, const Real* _vals, const unsigned _n, const Real _lambdaStart, const Real _lambdaEnd) {
	// Handle cases with out-of-bounds range or single sample only
	if (_n == 1 || _lambdaEnd <= _lambda[0]) return _vals[0];
	if (_lambdaStart >= _lambda[_n - 1]) return _vals[_n - 1];

	// Add contributions of constant segments before/after samples
	Real sum = 0.;
	if (_lambdaStart < _lambda[0]) sum += _vals[0] * (_lambda[0] - _lambdaStart);
	if (_lambdaEnd > _lambda[_n - 1]) sum += _vals[_n - 1] * (_lambdaEnd - _lambda[_n - 1]);

	// Advance to first relevant wavelength segment
	unsigned i = 0;
	while (_lambda[i + 1] < _lambdaStart) ++i;

	// Interpolate vals between first (i) and second (i+1) lambda and add that segment.
	const Real vs = maths::Lerp(_vals[i], _vals[i+1], (_lambdaStart - _lambda[i]) / (_lambda[i+1] - _lambda[i]));
	sum += .5 * (_lambda[i + 1] - _lambdaStart) * (vs + _vals[i+1]);
	++i;
	// Add contributions of middle segments that are not touching start/end boundary.
	for (; i < _n - 1 && _lambdaEnd >= _lambda[i]; ++i)
		sum += .5 * (_lambda[i + 1] - _lambda[i]) * (_vals[i] + _vals[i+1]);

	// Add end segment using end intepolated value.
	const Real ve = maths::Lerp(_vals[i], _vals[i+1], (_lambdaEnd - _lambda[i]) / (_lambda[i+1] - _lambda[i]));
	sum += .5 * (_lambda[i+1] - _lambda[i]) * (ve + _vals[i]);

	return sum / (_lambdaEnd - _lambdaStart);
}


class SampledSpectrum : public CoefficientSpectrum<nSpectralSamples> {
	public:
		SampledSpectrum(const Real _v = 0.) : CoefficientSpectrum(_v) {}

		static SampledSpectrum FromSampled(const Real* _lambda, const Real* _v, const unsigned _n) {
			SampledSpectrum tmp;
			for (unsigned i = 0; i < nSpectralSamples; ++i) {

			}
			return tmp;
		}
};

//typedef RGBSpectrum Spectrum;

//static Spectrum Lerp(const Spectrum &_a, const Spectrum &_b, const Real _r) {
//	return _a + (_b - _a) * _r;
//}