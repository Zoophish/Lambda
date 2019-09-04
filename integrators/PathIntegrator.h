#pragma once
#include "Integrator.h"
#include <shading/BxDF.h>

class PathIntegrator : public Integrator {
	public:
		unsigned maxBounces;

		PathIntegrator(Sampler *_sampler, const unsigned _maxBounces = 1024) {
			maxBounces = _maxBounces;
			sampler = _sampler;
		}

		Integrator *clone() const override { return new PathIntegrator(*this); }

		Spectrum Li(const Ray &_r, const Scene &_scene) const override {
			Spectrum L(0), beta(1);
			Ray r = _r;
			RayHit hit;
			SurfaceScatterEvent event;
			event.scene = &_scene;
			for (unsigned bounces = 0; bounces < maxBounces; ++bounces) {
				if (_scene.Intersect(r, hit)) {
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
						r.o = hit.point;
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