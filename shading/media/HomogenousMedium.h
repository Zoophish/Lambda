#pragma once
#include "Media.h"

LAMBDA_BEGIN

class HomogenousMedium : public Medium {
	public:
		Spectrum sigmaA, sigmaS, sigmaT;
		Real g;

		HomogenousMedium(const Real _g, const Spectrum &_sigmaA, const Spectrum &_sigmaS, const Spectrum &_sigmaT) {
			g = _g;
			sigmaA = _sigmaA;
			sigmaS = _sigmaS;
			sigmaT = _sigmaT;
		}

		Spectrum Tr(const Ray &_ray, const Real _tFar, Sampler &_sampler) const override {
			return Spectrum::Exp(-sigmaT * std::min(_tFar, MAX_REAL));
		}

		Spectrum Sample(const Ray &_ray, Sampler &_sampler, SurfaceScatterEvent &_event) const override {
			const unsigned channel = std::min((unsigned)(_sampler.Get1D() * Spectrum::nSamples), Spectrum::nSamples - 1);
			const Real dist = -std::log(1 - _sampler.Get1D()) / sigmaT[channel];
			const Real t = std::min(dist, _event.hit->tFar);
			const bool sampledMedium = t < _event.hit->tFar;
		
			// Compute the transmittance and sampling density
			Spectrum Tr = Spectrum::Exp(-sigmaT * std::min(t, MAX_REAL));

			// Return weighting factor for scattering from homogeneous medium
			Spectrum density = sampledMedium ? (sigmaT * Tr) : Tr;
			Real pdf = 0;
			for (unsigned i = 0; i < Spectrum::nSamples; ++i) pdf += density[i];
			pdf *= 1 / (Real)Spectrum::nSamples;
			if (pdf == 0) pdf = 1;
			return sampledMedium ? (Tr * sigmaS / pdf) : (Tr / pdf);
		}

		Real p(const Vec3 &_wo, const Vec3 &_wi) const override {
			return Media::PhaseHG(maths::Dot(_wo, _wi), g);
		}
};

LAMBDA_END