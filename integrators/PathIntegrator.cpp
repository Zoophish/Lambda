#include "PathIntegrator.h"

LAMBDA_BEGIN

PathIntegrator::PathIntegrator(Sampler *_sampler, const unsigned _maxBounces, const unsigned _minBounces) {
	maxBounces = _maxBounces;
	minBounces = _minBounces;
	sampler = _sampler;
}

Integrator *PathIntegrator::clone() const {
	return new PathIntegrator(*this);
}

Spectrum PathIntegrator::Li(Ray r, const Scene &_scene) const {
	Spectrum L(0), beta(1);
	RayHit hit;
	ScatterEvent event;
	event.hit = &hit;
	event.scene = &_scene;
	event.wo = -r.d;
	bool scatterIntersect = false;
	for (int bounces = 0; bounces < maxBounces; ++bounces) {
		if (bounces == 0 ? _scene.Intersect(r, hit) : scatterIntersect) {
			
			if (bounces == 0 && hit.object->light) L += hit.object->light->L(event); //Beta is always 1 here, so it is excluded from the product.

			if (hit.object->bxdf) {
				event.SurfaceLocalise();
				event.wo = -r.d;
				Real lightDistPdf = 1;
				const Light *l = _scene.lights[_scene.lightDistribution.SampleDiscrete(sampler->Get1D(), &lightDistPdf)];
				Spectrum Ld(0);
				Real scatteringPDF, lightPDF;
				Spectrum f, Li = l->Sample_Li(event, sampler, lightPDF);
				if (lightPDF > 0 && !Li.IsBlack()) {	//Add light sample contribution
					f = hit.object->bxdf->f(event) * std::abs(event.wiL.y);
					scatteringPDF = hit.object->bxdf->Pdf(event.woL, event.wiL, event);
					if (!f.IsBlack() && scatteringPDF > 0) {
						const Real weight = PowerHeuristic(1, lightPDF, 1, scatteringPDF);
						Ld += Li * f * weight / lightPDF;
					}
				}
				f = hit.object->bxdf->Sample_f(event, *sampler, scatteringPDF);
				f *= std::abs(event.wiL.y);	//This must follow previous line to allow computation of wiL.
				lightPDF = l->PDF_Li(event, *sampler);	//Evaluated before hit is altered to next path vertex
				r.o = hit.point;
				r.d = event.wi;
				scatterIntersect = _scene.Intersect(r, hit);	//Go to next path vertex
				if (scatteringPDF > 0 && !f.IsBlack()) {	//Add bsdf-scatter light contribution
					if (lightPDF > 0) {
						Li = Spectrum(0);
						const Real weight = PowerHeuristic(1, scatteringPDF, 1, lightPDF);
						if (scatterIntersect) {
							if (hit.object->light == l)
								Li = hit.object->light->L(event);
						}
						else {
							Li = l->Le(r);
						}
						if (!Li.IsBlack()) Ld += Li * f * weight / scatteringPDF;
					}
				}
				else break;	//Don't continue path if bsdf is 0 or if scattering pdf is 0

				L += beta * (Ld / lightDistPdf);
				beta *= f / scatteringPDF;
			}
			else {
				r.o = hit.point + hit.normalG * (maths::Dot(r.d, hit.normalG) < 0 ? -SURFACE_EPSILON : SURFACE_EPSILON);
				if (bounces > 0) scatterIntersect = _scene.Intersect(r, hit);
				bounces--;
				continue;
			}

			if (bounces > minBounces) {
				const Real q = std::max((Real).05, 1 - beta.y());
				if (sampler->Get1D() < q) break;
				beta /= (Real)1 - q;
			}
		}
		else {
			if (bounces == 0) L += _scene.envLight->Le(r); //Beta is always 1 here also.
			break;
		}
	}
	return L;
}

LAMBDA_END