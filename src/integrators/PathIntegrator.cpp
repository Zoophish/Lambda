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
			
			if (bounces == 0 && hit.object->material && hit.object->material->light) L += hit.object->material->light->L(event); //Beta is always 1 here, so it is excluded from the product.

			if (hit.object->material && hit.object->material->bxdf) {
				event.SurfaceLocalise();
				event.wo = -r.d;
				Real lightDistPdf = 1;	//Probability of choosing dicrete light
				const Light *l = _scene.lightSampler->Sample(event, *sampler, &lightDistPdf);
				Spectrum Ld(0);
				Real scatteringPDF, lightPDF;
				Spectrum f, Li = l->Sample_Li(event, sampler, lightPDF);
				lightPDF *= lightDistPdf;	//The full light pdf
				if (lightPDF > 0 && !Li.IsBlack()) {	//Add light sample contribution
					f = hit.object->material->bxdf->f(event) * std::abs(event.wiL.y);
					scatteringPDF = hit.object->material->bxdf->Pdf(event.woL, event.wiL, event);
					if (!f.IsBlack() && scatteringPDF > 0) {
						const Real weight = PowerHeuristic(lightPDF, scatteringPDF);
						Ld += Li * f * weight / lightPDF;
					}
				}
				f = hit.object->material->bxdf->Sample_f(event, *sampler, scatteringPDF);
				f *= std::abs(event.wiL.y);	//This must follow previous line to allow computation of wiL

				r.o = hit.point;
				r.d = event.wi;
				scatterIntersect = _scene.Intersect(r, hit);	//Go to next path vertex (also the bxdf light sample)
				if (scatteringPDF > 0 && !f.IsBlack()) {	//Add bsdf-scatter light contribution
					if (const Light *nl = scatterIntersect ? hit.object->material->light : (Light*)_scene.envLight) {	//Check a light was hit or infinite light is present

						if (nl != l) lightDistPdf = _scene.lightSampler->Pdf(event, nl); //Recalculate light distribution pdf if we don't already know it
						lightPDF = lightDistPdf * nl->PDF_Li(event);	//Full light pdf
						Li = Spectrum(0);
						if (scatterIntersect) Li = nl->L(event);
						else Li = nl->Le(r);	//Special case for infinite lights
						const Real weight = PowerHeuristic(scatteringPDF, lightPDF);
						if (!Li.IsBlack()) Ld += Li * f * weight / scatteringPDF;

					}
				}
				else break;	//Don't continue path if bsdf is 0 or if scattering pdf is 0

				L += beta * Ld;
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
			if (bounces == 0) L += ((Light*)_scene.envLight)->Le(r); //Beta is always 1 here also.
			break;
		}
	}
	return L;
}

LAMBDA_END