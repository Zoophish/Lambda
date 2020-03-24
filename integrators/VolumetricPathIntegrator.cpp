#include "VolumetricPathIntegrator.h"

LAMBDA_BEGIN

VolumetricPathIntegrator::VolumetricPathIntegrator(Sampler *_sampler, const unsigned _maxBounces, const unsigned _minBounces) {
	maxBounces = _maxBounces;
	minBounces = _minBounces;
	sampler = _sampler;
}

Integrator *VolumetricPathIntegrator::clone() const {
	return new VolumetricPathIntegrator(*this);
}

static Medium *InMedium(const Vec3 &_p, const Scene &_scene) {
	//Embree rtcCollide... this could be slow so maybe a bool option to turn this off?
	return nullptr;
}

Spectrum VolumetricPathIntegrator::Li(Ray r, const Scene &_scene) const {
	Spectrum L(0), beta(1);
	RayHit hit;
	SurfaceScatterEvent event;
	event.hit = &hit;
	event.scene = &_scene;
	event.wo = -r.d;
	bool scatterIntersect = false;
	bool newIntersect = true;
	event.medium = InMedium(r.o, _scene);
	for (int bounces = 0; bounces < maxBounces; ++bounces) {
		if (bounces == 0 && newIntersect ? _scene.Intersect(r, hit) : scatterIntersect) {

			if (bounces == 0) {	//Direct lighting on bounce 0 done here
				if (hit.object->light) {
					if (event.medium) {
						beta *= event.medium->Tr(r, hit.tFar, *sampler);
						L += hit.object->light->L(event) * beta;
					}
					else L += hit.object->light->L(event); //Beta is always 1 here, so it is excluded from the product
				}
			}

			if (event.medium) {
				event.medium->Sample(r, *sampler, event);
				if (!event.mediumInteraction) beta *= event.medium->Tr(r, hit.tFar, *sampler);
			}
			else event.mediumInteraction = false;

			if (!event.mediumInteraction) {
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
					event.medium = hit.object->mediaBoundary->GetMedium(event.wi, hit.normalG);	//Evaluate any medium we are going into before we get new hit

					scatterIntersect = _scene.Intersect(r, hit);	//Go to next path vertex
					newIntersect = false;

					if (scatteringPDF > 0 && !f.IsBlack()) {	//Add bsdf-scattering light contribution
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
					event.medium = hit.object->mediaBoundary->GetMedium(r.d, hit.normalG);	//Set medium for next ray
					r.o = hit.point + hit.normalG * (maths::Dot(event.hit->normalG, r.d) < 0 ? -SURFACE_EPSILON : SURFACE_EPSILON);
					scatterIntersect = _scene.Intersect(r, hit);	//Go to next path vertex, but...
					newIntersect = false;
					bounces--;	//don't consider it a bounce
					continue;
				}
			}
			else if(event.medium) {
				event.wo = -r.d;
				Real lightDistPdf = 1;
				const Light *l = _scene.lights[_scene.lightDistribution.SampleDiscrete(sampler->Get1D(), &lightDistPdf)];
				Spectrum Ld(0);
				Real scatteringPDF, lightPDF;
				Spectrum p, Li = l->Sample_Li(event, sampler, lightPDF);
				if (lightPDF > 0 && !Li.IsBlack()) {
					scatteringPDF = event.medium->phase->p(event.wo, event.wi);
					p = Spectrum(scatteringPDF);
					if (scatteringPDF > 0) {
						const Real weight = PowerHeuristic(1, lightPDF, 1, scatteringPDF);
						Ld += Li * p * weight / lightPDF;
					}
				}
				scatteringPDF = event.medium->phase->Sample_p(event.wo, &event.wi, *sampler);
				p = scatteringPDF;
				lightPDF = l->PDF_Li(event, *sampler);
				r.o = hit.point;
				r.d = event.wi;

				Spectrum Tr(1);
				scatterIntersect = _scene.IntersectTr(r, hit, *sampler, event.medium, &Tr);	//Direct lighting from phase scatter

				if (scatteringPDF > 0) {	//Add phase-scattering light contribution
					if (lightPDF > 0) {
						Li = Spectrum(0);
						const Real weight = PowerHeuristic(1, scatteringPDF, 1, lightPDF);
						if (scatterIntersect) {
							if (hit.object->light == l) Li = hit.object->light->L(event) * Tr;
						}
						else {
							Li = l->Le(r) * Tr;
						}
						if (!Li.IsBlack()) Ld += Li * p * weight / scatteringPDF;
					}
				}
 				else break;
				
				scatteringPDF = event.medium->phase->Sample_p(event.wo, &event.wi, *sampler);	//Sample phase for next path vertex
				p = Spectrum(scatteringPDF);
				r.o = hit.point;
				r.d = event.wi;

				scatterIntersect = _scene.Intersect(r, hit);	//Next path vertex
				newIntersect = false;

				L += beta * (Ld / lightDistPdf);
				beta *= p / scatteringPDF;
			}

			if (bounces > minBounces) {
				const Real q = std::max((Real).05, 1 - beta.y());
				if (sampler->Get1D() < q) break;
				beta /= (Real)1 - q;
			}
		}
		else {
			if (bounces == 0) L += _scene.envLight->Le(r) * beta; //Beta is not always 1 here (due to vol attenuation)
			break;
		}
	}
	return L;
}

LAMBDA_END