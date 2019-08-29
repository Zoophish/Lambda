#pragma once
#include "Integrator.h"
#include <shading/BxDF.h>

class PathIntegrator : public Integrator {
	public:
		unsigned maxBounces;

		PathIntegrator(Sampler *_sampler, const unsigned _maxBounces = 128) {
			maxBounces = _maxBounces;
			sampler = _sampler;
		}

		Spectrum Li(const Ray &_r, const Scene &_scene) const override {
			Spectrum L(0), beta(1);
			Ray r = _r;
			RayHit hit;
			for (unsigned bounces = 0; bounces < maxBounces; ++bounces) {
				if (_scene.Intersect(r, hit)) {

					SurfaceScatterEvent event;
					event.hit = &hit;
					event.scene = &_scene;
					event.wo = -r.d;

					if (bounces == 0 && hit.object->light) {
						L += hit.object->light->L(event);
					}

					if (hit.object->bxdf) {
						L += beta * SampleOneLight(event, _scene);
						const Spectrum f = hit.object->bxdf->Sample_f(event, sampler->Get2D(), event.pdf);
						beta *= f * std::abs(maths::Dot(hit.normalS, event.wi)) / event.pdf;
						r.o = hit.point + hit.normalG * .001;
						r.d = event.wi;
					}
					//else

					Real q = std::max((Real).05, 1 - beta.y());
					if (sampler->Get1D() < q) break;
					beta /= 1 - q;
					
				}
				else {
					L += _scene.envLight->Le(r);
					break;
				}
			}
			return L;
		}
};