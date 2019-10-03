#pragma once
#include "Integrator.h"

class UtilityIntegrator : public Integrator {
	public:
		virtual Colour P(const Ray &_ray, const Scene &_scene) const = 0;

		Spectrum Li(const Ray &_ray, const Scene &_scene) const override {
			const Colour c = P(_ray, _scene);
			const Real cr[3] = { c.r, c.g, c.b };
			return Spectrum::FromRGB(cr, SpectrumType::Reflectance);
		}
};

class DepthPass : public UtilityIntegrator {
	public:
		Real power, maxDepth;
		Colour background;

		DepthPass(Sampler *_sampler, const Real _maxDepth, const Real _power = 1, const Colour _background = Colour(1,0,1)) {
			sampler = _sampler;
			power = _power;
			maxDepth = _maxDepth;
			background = _background;
		}

		Integrator *clone() const override { return new DepthPass(*this); }

		Colour P(const Ray &_ray, const Scene &_scene) const override {
			RayHit hit;
			if (_scene.Intersect(_ray, hit)) {
				const Real depth = std::pow((hit.point - _ray.o).Magnitude() / maxDepth, power);
				return Colour(depth, depth, depth);
			}
			return background;
		}
};

class NormalPass : public UtilityIntegrator {
	public:
		bool normalG;

		Integrator *clone() const override { return new NormalPass(*this); }

		NormalPass(Sampler *_sampler, const bool _normalG = false) {
			sampler = _sampler;
			normalG = _normalG;
		}

		Colour P(const Ray &_ray, const Scene &_scene) const override {
			RayHit hit;
			if (_scene.Intersect(_ray, hit)) {
				const Vec3 n = normalG ? hit.normalG : hit.normalS;
				return Colour(n.x * .5 + .5, n.y * .5 + 1, n.z * .5 + .5);
			}
			return Colour(0,0,0);
		}
};