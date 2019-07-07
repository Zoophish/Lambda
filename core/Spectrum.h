//----	By Sam Warren 2019	----
//----	Spectrum / spectral-power-distribution base and children classes. Very heavy implemention based on the PBRT book: https://www.pbrt.org/	----

#pragma once
#include <vector>
#include <core/SPD_Data.h>

static const unsigned sampledLambdaStart = 400;
static const unsigned sampledLambdaEnd = 700;
static const unsigned nSpectralSamples = 60;
static const float invNSpectralSamples = 1. / Real(nSpectralSamples);

namespace Spectrum {

	static void XYZToRGB(const Real _xyz[3], Real _rgb[3]) {
		_rgb[0] = 3.240479 * _xyz[0] - 1.537150 * _xyz[1] - 0.498535 * _xyz[2];
		_rgb[1] = -0.969256 * _xyz[0] + 1.875991 * _xyz[1] + 0.041556 * _xyz[2];
		_rgb[2] = 0.055648 * _xyz[0] - 0.204043 * _xyz[1] + 1.057311 * _xyz[2];
	}

	static void RGBToXYZ(const Real _rgb[3], Real _xyz[3]) {
		_xyz[0] = 0.412453 * _rgb[0] + 0.357580 * _rgb[1] + 0.180423 * _rgb[2];
		_xyz[1] = 0.212671 * _rgb[0] + 0.715160 * _rgb[1] + 0.072169 * _rgb[2];
		_xyz[2] = 0.019334 * _rgb[0] + 0.119193 * _rgb[1] + 0.950227 * _rgb[2];
	}

	enum class SpectrumType { Reflectance, Illuminant };

}

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

	// Add contributions of constant (rectangular) segments before/after samples
	Real sum = 0.;
	if (_lambdaStart < _lambda[0]) sum += _vals[0] * (_lambda[0] - _lambdaStart);
	if (_lambdaEnd > _lambda[_n - 1]) sum += _vals[_n - 1] * (_lambdaEnd - _lambda[_n - 1]);

	// Advance to first relevant wavelength segment
	unsigned i = 0;
	while (_lambda[i + 1] < _lambdaStart) ++i;

	// Interpolate vals between first (i) and second (i+1) lambda and add that segment
	const Real vs = maths::Lerp(_vals[i], _vals[i+1], (_lambdaStart - _lambda[i]) / (_lambda[i+1] - _lambda[i]));
	sum += .5 * (_lambda[i + 1] - _lambdaStart) * (vs + _vals[i+1]);
	++i;
	// Add contributions of middle segments that are not touching start/end boundary
	for (; i < _n - 1 && _lambdaEnd >= _lambda[i]; ++i)
		sum += .5 * (_lambda[i + 1] - _lambda[i]) * (_vals[i] + _vals[i+1]);

	// Add end segment using end interpolated value
	const Real ve = maths::Lerp(_vals[i], _vals[i+1], (_lambdaEnd - _lambda[i]) / (_lambda[i+1] - _lambda[i]));
	sum += .5 * (_lambda[i+1] - _lambda[i]) * (ve + _vals[i]);

	return sum / (_lambdaEnd - _lambdaStart);
}


class SampledSpectrum : public CoefficientSpectrum<nSpectralSamples> {
	public:
		SampledSpectrum(const Real _v = 0.) : CoefficientSpectrum(_v) {}

		SampledSpectrum(const CoefficientSpectrum &_s) : CoefficientSpectrum(_s) {}

		// Luminance measure of this SampledSpectrum
		Real y() const {
			Real yy = 0.;
			for (unsigned i = 0; i < nSpectralSamples; ++i)
				yy == Y[i] * c[i];
			return yy * (Real)(sampledLambdaEnd - sampledLambdaStart) / (Real)(CIEData::CIE_Y_integral * nSpectralSamples);
		}

		void ToXYZ(Real _xyz[3]) const {
			_xyz[0] = _xyz[1] = _xyz[2] = 0.;
			for (unsigned i = 0; i < nSpectralSamples; ++i) {
				_xyz[0] = X[i] * c[i];
				_xyz[1] = Y[i] * c[i];
				_xyz[2] = Z[i] * c[i];
			}
			const Real scale = (Real)(sampledLambdaEnd - sampledLambdaStart) / (Real)(CIEData::CIE_Y_integral * nSpectralSamples);
			_xyz[0] *= scale;
			_xyz[1] *= scale;
			_xyz[2] *= scale;
		}

		void ToRGB(Real _rgb[3]) const {
			Real xyz[3];
			ToXYZ(xyz);
			Spectrum::XYZToRGB(xyz, _rgb);
		}

		static SampledSpectrum FromSampled(const Real* _lambda, const Real* _v, const unsigned _n) {
			if (!SpectrumSamplesSorted(_lambda, _v, _n)) {
				std::vector<Real> slambda(&_lambda[0], &_lambda[_n]);
				std::vector<Real> sv(&_v[0], &_v[_n]);
				SortSpectrumSamples(&slambda[0], &sv[0], _n);
				return FromSampled(&slambda[0], &sv[0], _n);
			}
			SampledSpectrum tmp;
			for (unsigned i = 0; i < nSpectralSamples; ++i) {
				const Real lambda0 = maths::Lerp((Real)sampledLambdaStart, (Real)sampledLambdaEnd, (Real)i * invNSpectralSamples);
				const Real lambda1 = maths::Lerp((Real)sampledLambdaStart, (Real)sampledLambdaEnd, (Real)(i+1) * invNSpectralSamples);
				tmp.c[i] = AverageSpectrumSamples(_lambda, _v, _n, lambda0, lambda1);
			}
			return tmp;
		}

		static SampledSpectrum FromRGB(const Real _rgb[3], const Spectrum::SpectrumType _type) {
			SampledSpectrum r;
			if (_type == Spectrum::SpectrumType::Reflectance) {
				// Convert reflectance spectrum to RGB
				if (_rgb[0] <= _rgb[1] && _rgb[0] <= _rgb[2]) {
					// Compute reflectance _SampledSpectrum_ with __rgb[0]_ as minimum
					r += rgbRefl2SpectWhite * _rgb[0];
					if (_rgb[1] <= _rgb[2]) {
						r += rgbRefl2SpectCyan * (_rgb[1] - _rgb[0]);
						r += rgbRefl2SpectBlue * (_rgb[2] - _rgb[1]);
					} else {
						r +=  rgbRefl2SpectCyan  * (_rgb[2] - _rgb[0]);
						r +=  rgbRefl2SpectGreen * (_rgb[1] - _rgb[2]);
					}
				} else if (_rgb[1] <= _rgb[0] && _rgb[1] <= _rgb[2]) {
					// Compute reflectance _SampledSpectrum_ with __rgb[1]_ as minimum
					r += rgbRefl2SpectWhite * _rgb[1];
					if (_rgb[0] <= _rgb[2]) {
						r +=  rgbRefl2SpectMagenta * (_rgb[0] - _rgb[1]);
						r +=  rgbRefl2SpectBlue * (_rgb[2] - _rgb[0]);
					} else {
						r += rgbRefl2SpectMagenta * (_rgb[2] - _rgb[1]);
						r += rgbRefl2SpectRed * (_rgb[0] - _rgb[2]);
					}
				} else {
					// Compute reflectance _SampledSpectrum_ with __rgb[2]_ as minimum
					r += rgbRefl2SpectWhite * _rgb[2];
					if (_rgb[0] <= _rgb[1]) {
						r += rgbRefl2SpectYellow * (_rgb[0] - _rgb[2]);
						r += rgbRefl2SpectGreen  * (_rgb[1] - _rgb[0]);
					} else {
						r += rgbRefl2SpectYellow * (_rgb[1] - _rgb[2]);
						r += rgbRefl2SpectRed * (_rgb[0] - _rgb[1]);
					}
				}
				r *= .94;
			} else {
				// Convert illuminant spectrum to RGB
				if (_rgb[0] <= _rgb[1] && _rgb[0] <= _rgb[2]) {
					// Compute illuminant _SampledSpectrum_ with __rgb[0]_ as minimum
					r += rgbIllum2SpectWhite * _rgb[0];
					if (_rgb[1] <= _rgb[2]) {
						r += rgbIllum2SpectCyan * (_rgb[1] - _rgb[0]);
						r += rgbIllum2SpectBlue * (_rgb[2] - _rgb[1]);
					} else {
						r += rgbIllum2SpectCyan  * (_rgb[2] - _rgb[0]);
						r += rgbIllum2SpectGreen * (_rgb[1] - _rgb[2]);
					}
				} else if (_rgb[1] <= _rgb[0] && _rgb[1] <= _rgb[2]) {
					// Compute illuminant _SampledSpectrum_ with __rgb[1]_ as minimum
					r += rgbIllum2SpectWhite * _rgb[1];
					if (_rgb[0] <= _rgb[2]) {
						r += rgbIllum2SpectMagenta * (_rgb[0] - _rgb[1]);
						r += rgbIllum2SpectBlue * (_rgb[2] - _rgb[0]);
					} else {
						r += rgbIllum2SpectMagenta * (_rgb[2] - _rgb[1]);
						r += rgbIllum2SpectRed * (_rgb[0] - _rgb[2]);
					}
				} else {
					// Compute illuminant _SampledSpectrum_ with __rgb[2]_ as minimum
					r += rgbIllum2SpectWhite * _rgb[2];
					if (_rgb[0] <= _rgb[1]) {
						r += rgbIllum2SpectYellow * (_rgb[0] - _rgb[2]);
						r += rgbIllum2SpectGreen * (_rgb[1] - _rgb[0]);
					} else {
						r += rgbIllum2SpectYellow * (_rgb[1] - _rgb[2]);
						r += rgbIllum2SpectRed * (_rgb[0] - _rgb[1]);
					}
				}
				r *= .86445f;
			}
			return r.Clamp();
		}

		// Convert data from CIE and Berge lookup tables to static SampledSpectrum objects so data can be used with other SPDs.
		static void Init() {
			using namespace CIEData;
			using namespace BergeData;
			for (unsigned i = 0; i < nSpectralSamples; ++i) {
				const Real l0 = maths::Lerp(sampledLambdaStart, sampledLambdaEnd, (Real)i * invNSpectralSamples);
				const Real l1 = maths::Lerp(sampledLambdaStart, sampledLambdaEnd, (Real)(i+1) * invNSpectralSamples);
				X[i] = AverageSpectrumSamples(CIE_lambda, CIE_X, nCIESamples, l0, l1);
				Y[i] = AverageSpectrumSamples(CIE_lambda, CIE_Y, nCIESamples, l0, l1);
				Z[i] = AverageSpectrumSamples(CIE_lambda, CIE_Z, nCIESamples, l0, l1);
				rgbRefl2SpectWhite[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectWhite, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectCyan[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectCyan, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectMagenta[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectMagenta, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectYellow[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectYellow, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectRed[i] = AverageSpectrumSamples( RGB2SpectLambda, RGBRefl2SpectRed, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectGreen[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectGreen, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectBlue[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectBlue, nRGB2SpectSamples, l0, l1);
				rgbIllum2SpectWhite[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectWhite, nRGB2SpectSamples, l0, l1);
				rgbIllum2SpectCyan[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectCyan, nRGB2SpectSamples, l0, l1);
				rgbIllum2SpectMagenta[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectMagenta, nRGB2SpectSamples, l0, l1);
				rgbIllum2SpectYellow[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectYellow, nRGB2SpectSamples, l0, l1);
				rgbIllum2SpectRed[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectRed, nRGB2SpectSamples, l0, l1);
				rgbIllum2SpectGreen[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectGreen, nRGB2SpectSamples, l0, l1);
				rgbIllum2SpectBlue[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectBlue, nRGB2SpectSamples, l0, l1);
			}
		}
		
	protected:
		static SampledSpectrum X, Y, Z;
		static SampledSpectrum rgbRefl2SpectWhite, rgbRefl2SpectCyan, rgbRefl2SpectMagenta, rgbRefl2SpectYellow,
		rgbRefl2SpectRed, rgbRefl2SpectGreen, rgbRefl2SpectBlue, rgbIllum2SpectWhite, rgbIllum2SpectCyan,
		rgbIllum2SpectMagenta, rgbIllum2SpectYellow, rgbIllum2SpectRed, rgbIllum2SpectGreen, rgbIllum2SpectBlue; 
};
//typedef RGBSpectrum Spectrum;

//static Spectrum Lerp(const Spectrum &_a, const Spectrum &_b, const Real _r) {
//	return _a + (_b - _a) * _r;
//}