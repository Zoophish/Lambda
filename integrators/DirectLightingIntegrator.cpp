#include "DirectLightingIntegrator.h"

DirectLightingIntegrator::DirectLightingIntegrator(Sampler *_sampler, const unsigned _maxDepth) : maxDepth(_maxDepth) {
	sampler = _sampler;
}

Integrator *DirectLightingIntegrator::clone() const {
	return new DirectLightingIntegrator(*this);
}

Spectrum DirectLightingIntegrator::Li(const Ray &_ray, const Scene &_scene) const {
	RayHit hit;
	if (_scene.Intersect(_ray, hit)) {
		if (hit.object->bxdf) {
			SurfaceScatterEvent event;
			event.hit = &hit;
			event.scene = &_scene;
			event.wo = -_ray.d;
			event.Localise();
			return SampleOneLight(event, _scene);
		}
		else {
			return Li(Ray(hit.point + _ray.d * .0001, _ray.d), _scene);
		}
	}
	return _scene.envLight->Le(_ray);
}