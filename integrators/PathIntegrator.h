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
			SurfaceScatterEvent event;	//Reusable object to reduce instantiation and destruction.
			event.scene = &_scene;
			event.isect = false;
			for (unsigned bounces = 0; bounces < maxBounces; ++bounces) {
				if (event.isect || _scene.Intersect(r, hit)) {
					event.hit = &hit;
					event.wo = -r.d;
					event.pdf = 1;

					if (bounces == 0 && hit.object->light) {
						L += beta * hit.object->light->L(event);
					}

					if (hit.object->bxdf) {
						event.Localise();
						L += beta * SampleOneLight(event, _scene);
						const Spectrum f = hit.object->bxdf->Sample_f(event, sampler->Get2D(), event.pdf);
						if ((event.pdf == 0) || f.IsBlack()) break;
						beta *= f * std::abs(event.wiL.y) / event.pdf;
						r.o = hit.point + hit.normalG * .00001;
						r.d = event.wi;
					}
					else {
						r.o = event.hit->point + r.d * .0005;
						continue;
					}

					if (bounces > 3) {
						Real q = std::max((Real).05, 1 - beta.y());
						if (sampler->Get1D() < q) break;
						beta /= (Real)1 - q;
					}
					
				}
				else {
					L += beta * _scene.envLight->Le(r);
					break;
				}
			}
			return L;
		}
};