#pragma once
#include "Integrator.h"

LAMBDA_BEGIN

class DirectLightingIntegrator : public Integrator {
	public:
		DirectLightingIntegrator(Sampler *_sampler, const unsigned _maxDepth = 4);

		Integrator *clone() const override;

		Spectrum Li(Ray _ray, const Scene &_scene) const override;

	protected:
		const unsigned maxDepth;
};

LAMBDA_END