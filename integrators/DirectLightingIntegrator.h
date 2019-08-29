#pragma once
#include "Integrator.h"
#include <core/Scene.h>
#include <sampling/Sampler.h>
#include <core/Ray.h>
#include <shading/BxDF.h>

enum class LightStrategy {
	SAMPLE_ONE,
	SAMPLE_ALL
};

class DirectLightingIntegrator : public Integrator {
	public:
		DirectLightingIntegrator(Sampler *_sampler, const unsigned _maxDepth = 4) : maxDepth(_maxDepth) {
			sampler = _sampler;
		}

		Spectrum Li(const Ray &_ray, const Scene &_scene) const override {
			RayHit hit;
			if (_scene.Intersect(_ray, hit)) {
				SurfaceScatterEvent event;
				event.hit = &hit;
				event.scene = &_scene;
				event.wo = -_ray.d;
				return SampleOneLight(event, _scene);
			}
			return _scene.envLight->Le(_ray);
		}

	protected:
		LightStrategy strategy;
		const unsigned maxDepth;
};