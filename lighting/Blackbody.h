/* Use for the creation of realistic light emission spectra based on a temperature in Kelvin.
Provides a much more realistic emission spectra than RGB lighting for temperature lights.
*/

#pragma once
#include <memory>
#include <core/Spectrum.h>

namespace Blackbody {

	//Function of relative intensity of emission spectra of ideal blackbody emitter at given temperature T in  Kelvin. (Stefan-Boltzmann)
	static void Blackbody(const Real *_lambda, int _n, Real _T, Real *_Le) {
		const Real c = 299792458;
		const Real h = 6.62606957e-34;
		const Real kb = 1.3806488e-23;
		for (unsigned i = 0; i < _n; ++i) {
			const Real l = _lambda[i] * 1e-9;
			const Real lambda5 = (l * l) * (l * l) * l;
			_Le[i] = (2 * h * c * c) / (lambda5 * (std::exp((h * c) / (l * kb * _T)) - 1));
		}
	}

	static void BlackbodyNormalized(const Real *_lambda, int _n, Real _T, Real *_Le) {
		Blackbody(_lambda, _n, _T, _Le);
		Real lambdaMax = 2.8977721e-3 / _T * 1e9;
		Real maxL;
		Blackbody(&lambdaMax, 1, _T, &maxL);
		for (int i = 0; i < _n; ++i) _Le[i] /= maxL;
	}

	static Spectrum MakeBlackbodySpectra(const Real _T) {
		std::unique_ptr<Real[]> lambdas(new Real[nSpectralSamples]);
		const int interval = sampledLambdaEnd - sampledLambdaStart;
		for (unsigned i = 0; i < Spectrum::nSamples; ++i) {
			lambdas[i] = (Real)sampledLambdaStart + (Real)interval * invNSpectralSamples * (Real)i;
		}
		std::unique_ptr<Real[]> v(new Real[nSpectralSamples]);
		BlackbodyNormalized(&lambdas[0], nSpectralSamples, _T, &v[0]);
		return Spectrum::FromSampled(&lambdas[0], &v[0], nSpectralSamples);
	}
}