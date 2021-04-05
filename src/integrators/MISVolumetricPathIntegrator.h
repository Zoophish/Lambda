#pragma once
#include "Integrator.h"


LAMBDA_BEGIN

class MISVolumetricPathIntegrator : public Integrator {
	public:
		unsigned maxBounces, minBounces;

		MISVolumetricPathIntegrator(Sampler *_sampler, const unsigned _maxBounces = 128, const unsigned _minBounces = 3);

		Integrator *clone() const override;

		/*
			Evaluate direct lighting at point _p in medium.
		*/
		Spectrum LdMediumPoint(ScatterEvent &_event, const Vec3 &_p, const Light &_l, Real _lpdf, Real *_f) const;

		Spectrum Li(Ray _r, const Scene &_scene) const override;
};

LAMBDA_END