#pragma once
#include "Integrator.h"

LAMBDA_BEGIN

class UtilityIntegrator : public Integrator {
	public:
		virtual Colour P(const Ray &_ray, const Scene &_scene) const = 0;

		Spectrum Li(Ray _ray, const Scene &_scene) const override;
};



class DepthPass : public UtilityIntegrator {
	public:
		Real power, maxDepth;
		Colour background;

		DepthPass(Sampler *_sampler, const Real _maxDepth, const Real _power = 1, const Colour _background = Colour(1, 0, 1));

		Integrator *clone() const override;

		Colour P(const Ray &_ray, const Scene &_scene) const override;
};



class NormalPass : public UtilityIntegrator {
	public:
		bool normalG;

		Integrator *clone() const override;

		NormalPass(Sampler *_sampler, const bool _normalG = false);

		Colour P(const Ray &_ray, const Scene &_scene) const override;
};



class AlbedoPass : public UtilityIntegrator {
	public:
		Integrator *clone() const override;

		AlbedoPass(Sampler *_sampler);

		Colour P(const Ray &_ray, const Scene &_scene) const override;
};

LAMBDA_END