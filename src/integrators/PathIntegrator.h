#pragma once
#include "Integrator.h"

LAMBDA_BEGIN

class PathIntegrator : public Integrator {
	public:
		unsigned maxBounces, minBounces;

		PathIntegrator(Sampler *_sampler, const unsigned _maxBounces = 128, const unsigned _minBounces = 3);

		Integrator *clone() const override;

		Spectrum Li(Ray _r, const Scene &_scene) const override;
};

LAMBDA_END