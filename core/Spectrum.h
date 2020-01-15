//----	By Sam Warren 2019	----
//----	Spectrum / spectral-power-distribution base and children classes. Very heavy implemention based on the PBRT book: https://www.pbrt.org/	----

#pragma once
#include <vector>
#include <omp.h>
#include "SPD_Data.h"

static const unsigned sampledLambdaStart = 400;
static const unsigned sampledLambdaEnd = 700;
static const unsigned nSpectralSamples = 32;
static const Real invNSpectralSamples = 1. / Real(nSpectralSamples);

enum class SpectrumType { Reflectance, Illuminant };

class RGBSpectrum;
class SampledSpectrum;

//	Define which type of spectrum is used in Lambda here.
//	 -	RGBSpectrum for performance.
//	 -	SampledSpectrum for accuracy.
typedef RGBSpectrum Spectrum;
//typedef SampledSpectrum Spectrum;

template<unsigned spectrumSamples>
class CoefficientSpectrum {
	public:
		static const unsigned nSamples = spectrumSamples;

		CoefficientSpectrum(const Real _v = 0) {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] = _v;
		}
		
		Real &operator[](const unsigned _i) {
			return c[_i];
		}
		Real operator[](const unsigned _i) const {
			return c[_i];
		}

		CoefficientSpectrum& operator+=(const CoefficientSpectrum &_s) {
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] += _s[i];
			return *this;
		}
		CoefficientSpectrum& operator-=(const CoefficientSpectrum &_s) {
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] -= _s[i];
			return *this;
		}
		CoefficientSpectrum& operator*=(const CoefficientSpectrum &_s) {
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] *= _s[i];
			return *this;
		}
		CoefficientSpectrum& operator/=(const CoefficientSpectrum &_s) {
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				c[i] /= _s[i];
			return *this;
		}

		CoefficientSpectrum operator+(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp = *this;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] += _s[i];
			return tmp;
		}
		CoefficientSpectrum operator-(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp = *this;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] -= _s[i];
			return tmp;
		}
		CoefficientSpectrum operator*(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp = *this;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] *= _s[i];
			return tmp;
		}
		CoefficientSpectrum operator/(const CoefficientSpectrum &_s) const {
			CoefficientSpectrum tmp = *this;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] /= _s[i];
			return tmp;
		}
		CoefficientSpectrum operator+(const Real _s) const {
			CoefficientSpectrum tmp = *this;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] += _s;
			return tmp;
		}
		CoefficientSpectrum operator-(const Real _s) const {
			CoefficientSpectrum tmp = *this;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] -= _s;
			return tmp;
		}
		CoefficientSpectrum operator-() const {
			CoefficientSpectrum tmp = *this;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] *= -1;
			return tmp;
		}
		CoefficientSpectrum operator*(const Real _s) const {
			CoefficientSpectrum tmp = *this;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] *= _s;
			return tmp;
		}
		CoefficientSpectrum operator/(const Real _s) const {
			const Real inv = 1. / _s;
			CoefficientSpectrum tmp = *this;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp[i] *= inv;
			return tmp;
		}

		bool IsBlack() const {
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				if (c[i] != 0) return false;
			return true;
		}
		bool HasNaNs() const {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				if (std::isnan(c[i])) return true;
			return false;
		}

		bool HasNegs() const {
			for (unsigned i = 0; i < spectrumSamples; ++i)
				if (c[i] < 0) return true;
			return false;
		}

		static CoefficientSpectrum Sqrt(const CoefficientSpectrum &_s) {
			CoefficientSpectrum tmp;
			#pragma omp simd
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp.c[i] = std::sqrt(_s.c[i]);
			return tmp;
		}
		static CoefficientSpectrum Pow(const CoefficientSpectrum &_s, const Real _p) {
			CoefficientSpectrum tmp;
			for (unsigned i = 0; i < spectrumSamples; ++i)
				tmp.c[i] = std::pow(_s.c[i], _p);
			return tmp;
		}
		static CoefficientSpectrum Exp(const CoefficientSpectrum &_s) {
			CoefficientSpectrum tmp;
			#pragma omp simd
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

extern bool SpectrumSamplesSorted(const Real* _lambda, const Real* _vals, const unsigned _n);

extern void SortSpectrumSamples(Real* _lambda, Real* _vals, const unsigned _n);

extern Real AverageSpectrumSamples(const Real* _lambda, const Real* _vals, const unsigned _n, const Real _lambdaStart, const Real _lambdaEnd);

extern Real InterpolateSpectrumSamples(const Real* _lambda, const Real* _vals, const unsigned _n, const Real _l);

namespace SpectrumUtils {

	inline void XYZToRGB(const Real _xyz[3], Real _rgb[3]) {
		_rgb[0] = 3.240479 * _xyz[0] - 1.537150 * _xyz[1] - 0.498535 * _xyz[2];
		_rgb[1] = -0.969256 * _xyz[0] + 1.875991 * _xyz[1] + 0.041556 * _xyz[2];
		_rgb[2] = 0.055648 * _xyz[0] - 0.204043 * _xyz[1] + 1.057311 * _xyz[2];
	}

	inline void RGBToXYZ(const Real _rgb[3], Real _xyz[3]) {
		_xyz[0] = 0.412453 * _rgb[0] + 0.357580 * _rgb[1] + 0.180423 * _rgb[2];
		_xyz[1] = 0.212671 * _rgb[0] + 0.715160 * _rgb[1] + 0.072169 * _rgb[2];
		_xyz[2] = 0.019334 * _rgb[0] + 0.119193 * _rgb[1] + 0.950227 * _rgb[2];
	}

	template<unsigned n>
	inline CoefficientSpectrum<n> Lerp(const CoefficientSpectrum<n>& _a, const CoefficientSpectrum<n>& _b, const Real _r) {
		return _a + (_b - _a) * _r;
	}
}

class SampledSpectrum : public CoefficientSpectrum<nSpectralSamples> {
	public:
		SampledSpectrum(const Real _v = 0.) : CoefficientSpectrum(_v) {}

		SampledSpectrum(const CoefficientSpectrum &_s) : CoefficientSpectrum(_s) {}

		// Luminance measure of this SampledSpectrum
		Real y() const {
			Real yy = 0.;
			for (unsigned i = 0; i < nSpectralSamples; ++i)
				yy = Y[i] * c[i];
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
			SpectrumUtils::XYZToRGB(xyz, _rgb);
		}

		// Convert data from CIE and Berge lookup-tables to static SampledSpectrum objects they can be used with other SPDs.
		static void Init() {
			using namespace CIEData;
			using namespace BergeData;
			for (unsigned i = 0; i < nSpectralSamples; ++i) {
				const Real l0 = maths::Lerp(sampledLambdaStart, sampledLambdaEnd, (Real)i * invNSpectralSamples);
				const Real l1 = maths::Lerp(sampledLambdaStart, sampledLambdaEnd, (Real)(i + 1) * invNSpectralSamples);
				X[i] = AverageSpectrumSamples(CIE_lambda, CIE_X, nCIESamples, l0, l1);
				Y[i] = AverageSpectrumSamples(CIE_lambda, CIE_Y, nCIESamples, l0, l1);
				Z[i] = AverageSpectrumSamples(CIE_lambda, CIE_Z, nCIESamples, l0, l1);
				rgbRefl2SpectWhite[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectWhite, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectCyan[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectCyan, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectMagenta[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectMagenta, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectYellow[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectYellow, nRGB2SpectSamples, l0, l1);
				rgbRefl2SpectRed[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectRed, nRGB2SpectSamples, l0, l1);
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
				tmp[i] = AverageSpectrumSamples(_lambda, _v, _n, lambda0, lambda1);
			}
			return tmp;
		}

		RGBSpectrum ToRGBSpectrum() const;

		static SampledSpectrum FromRGB(const Real _rgb[3], const SpectrumType _type);

		static SampledSpectrum FromXYZ(const Real _xyz[3], const SpectrumType _type = SpectrumType::Reflectance) {
			Real rgb[3];
			SpectrumUtils::XYZToRGB(_xyz, rgb);
			return FromRGB(rgb, _type);
		}

		SampledSpectrum(const RGBSpectrum &_r, SpectrumType _type = SpectrumType::Reflectance);

	private:
		static SampledSpectrum X, Y, Z, rgbRefl2SpectWhite, rgbRefl2SpectCyan, rgbRefl2SpectMagenta, rgbRefl2SpectYellow,
		rgbRefl2SpectRed, rgbRefl2SpectGreen, rgbRefl2SpectBlue, rgbIllum2SpectWhite, rgbIllum2SpectCyan,
		rgbIllum2SpectMagenta, rgbIllum2SpectYellow, rgbIllum2SpectRed, rgbIllum2SpectGreen, rgbIllum2SpectBlue; 
};

class alignas(16) RGBSpectrum : public CoefficientSpectrum<3> {
	using CoefficientSpectrum<3>::c;

	public:
		RGBSpectrum(const Real _v = 0.) : CoefficientSpectrum(_v) {}

		RGBSpectrum(const CoefficientSpectrum<3> &_s) : CoefficientSpectrum<3>(_s) {}

		RGBSpectrum(const RGBSpectrum &_s, SpectrumType _type = SpectrumType::Reflectance) {
			*this = _s;
		}

		void ToRGB(Real *_rgb) const {
			_rgb[0] = c[0];
			_rgb[1] = c[1];
			_rgb[2] = c[2];
		}

		void ToXYZ(Real _xyz[3]) const {
			SpectrumUtils::RGBToXYZ(c, _xyz);
		}

		Real y() const {
			const Real YWeight[3] = { 0.212671f, 0.715160f, 0.072169f };
			return YWeight[0] * c[0] + YWeight[1] * c[1] + YWeight[2] * c[2];
		}

		Real y_fast() const {
			return std::max(std::max(c[0], c[1]), c[2]);
		}

		const RGBSpectrum &ToRGBSpectrum() const { return *this; }

		static RGBSpectrum FromRGB(const Real _rgb[3], SpectrumType _type = SpectrumType::Reflectance) {
			RGBSpectrum s;
			s[0] = _rgb[0];
			s[1] = _rgb[1];
			s[2] = _rgb[2];
			return s;
		}

		static RGBSpectrum FromXYZ(const Real _xyz[3], SpectrumType type = SpectrumType::Reflectance) {
			RGBSpectrum r;
			SpectrumUtils::XYZToRGB(_xyz, r.c);
			return r;
		}

		static RGBSpectrum FromSampled(const Real *_lambda, const Real *_v, const unsigned _n) {
			using namespace CIEData;
			if (!SpectrumSamplesSorted(_lambda, _v, _n)) {
				std::vector<Real> slambda(&_lambda[0], &_lambda[_n]);
				std::vector<Real> sv(&_v[0], &_v[_n]);
				SortSpectrumSamples(&slambda[0], &sv[0], _n);
				return FromSampled(&slambda[0], &sv[0], _n);
			}
			Real xyz[3] = {0, 0, 0};
			for (unsigned i = 0; i < nCIESamples; ++i) {
				Real val = InterpolateSpectrumSamples(_lambda, _v, _n, CIE_lambda[i]);
				xyz[0] += val * CIE_X[i];
				xyz[1] += val * CIE_Y[i];
				xyz[2] += val * CIE_Z[i];
			}
			Real scale = Real(CIE_lambda[nCIESamples - 1] - CIE_lambda[0]) / Real(CIE_Y_integral * nCIESamples);
			xyz[0] *= scale;
			xyz[1] *= scale;
			xyz[2] *= scale;
			return FromXYZ(xyz);
		}
};