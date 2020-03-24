#pragma once
#include "Media.h"

LAMBDA_BEGIN

class HomogeneousMedium : public Medium {
	public:
		Spectrum sigmaA, sigmaS;

		HomogeneousMedium(const Spectrum &_sigmaA, const Spectrum &_sigmaS) {
			sigmaA = _sigmaA;
			sigmaS = _sigmaS;
		}

		Spectrum Tr(const Ray &_ray, const Real _tFar, Sampler &_sampler) const override {
			const Spectrum sigmaT = sigmaA + sigmaS;
			return Spectrum::Exp(-sigmaT * std::min(_tFar, MAX_REAL));
		}

		Spectrum Sample(const Ray &_ray, Sampler &_sampler, ScatterEvent &_event) const override {
			const Spectrum sigmaT = sigmaA + sigmaS;
			const unsigned channel = std::min((unsigned)(_sampler.Get1D() * Spectrum::nSamples), Spectrum::nSamples - 1);
			const Real dist = -std::log(1 - _sampler.Get1D()) / sigmaT[channel];
			const Real t = std::min(dist, _event.hit->tFar);
			_event.mediumInteraction = t < _event.hit->tFar;
			
			if (_event.mediumInteraction) _event.hit->point = _ray.o + _ray.d * t;

			Spectrum Tr = Spectrum::Exp(-sigmaT * std::min(t, MAX_REAL));

			// Return weighting factor for scattering from homogeneous medium
			Spectrum density = _event.mediumInteraction ? (sigmaT * Tr) : Tr;
			Real pdf = 0;
			for (unsigned i = 0; i < Spectrum::nSamples; ++i) pdf += density[i];
			pdf *= 1 / (Real)Spectrum::nSamples;
			if (pdf == 0) pdf = 1;
			return _event.mediumInteraction ? (Tr * sigmaS / pdf) : (Tr / pdf);
		}
};

LAMBDA_END