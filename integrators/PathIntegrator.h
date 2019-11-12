#pragma once
#include "Integrator.h"

class PathIntegrator : public Integrator {
	public:
		unsigned maxBounces, minBounces;

		PathIntegrator(Sampler *_sampler, const unsigned _maxBounces = 128, const unsigned _minBounces = 3);

		Integrator *clone() const override;

		Spectrum Li(const Ray &_r, const Scene &_scene) const override;
};