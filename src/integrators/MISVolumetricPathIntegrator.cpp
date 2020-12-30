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
	//Embree rtcCollide... what medium is the camera in?
	return nullptr;
}

Spectrum VolumetricPathIntegrator::Li(Ray r, const Scene &_scene) const {
	Spectrum L(0), beta(1);
	RayHit hit;
	ScatterEvent event;
	event.hit = &hit;
	event.scene = &_scene;
	event.wo = -r.d;
	bool scatterIntersect = false;
	event.medium = InMedium(r.o, _scene);
	for (int bounces = 0; bounces < maxBounces; ++bounces) {
		if (bounces == 0 ? _scene.Intersect(r, *event.hit) : scatterIntersect) {

			if (bounces == 0) {	//Direct lighting on bounce 0 done here
				if (hit.object->material->light) {
					if (event.medium) {
						L += hit.object->material->light->L(event) * event.medium->Tr(r, hit.tFar, *sampler);
					}
					else L += hit.object->material->light->L(event); //Beta is always 1 here, so it is excluded from the product
				}
			}

			//if (event.medium) beta *= event.medium->SampleDistance(r, *sampler, event);
			//else event.mediumInteraction = false;

			if (event.medium) {
				Real lightPDF, lightDistPdf = 1;
				const Light *l = _scene.lightSampler->Sample(event, *sampler, &lightDistPdf);	//TODO: Sample via ray segment
				lightPDF *= lightDistPdf; //Full light pdf
				Real lightPointPdf;
				const Vec3 lightPoint = l->SamplePoint(*sampler, event, &lightPointPdf);
				lightPDF *= lightPointPdf;

				Spectrum Ld(0);

				/*
					Distance Generators
				*/
				Real distanceT, distancePDF, equiangularPDF;
				const Spectrum trDistance = event.medium->SampleDistance(r, *sampler, event, &distanceT, &distancePDF);	//IMPORTANT: _pdf parameter may be redundant?
				const Vec3 distancePoint = r.o + r.d * distanceT;
				equiangularPDF = event.medium->PDFEquiangular(r, lightPoint, event.hit->tFar, (r.o - distancePoint).Magnitude());

				//	NEE
				Spectrum f;

				Ray neeRay(distancePoint, (lightPoint - distancePoint).Magnitude());
				RayHit neeHit;
				Spectrum neeTr(1), Li;
				Real scatterPDF;
				lightPDF = Light::PointMutualVisibility(distancePoint, lightPoint, event, _scene, *sampler, &neeTr) ? lightPDF : 0;
				if (lightPDF) {
					Li = l->L(event);
					f = event.mediumInteraction ? event.medium->phase->p(event.wo, event.wi) : hit.object->material->bxdf->f(event) * std::abs(event.wiL.y);
					scatterPDF = event.mediumInteraction ? f[0] : hit.object->material->bxdf->Pdf(event.woL, event.wiL, event);
					if (!f.IsBlack() && scatterPDF > 0) {
						const Real weight = PowerHeuristic(1, lightPDF, 1, scatterPDF);
						Ld += Li * neeTr * f * weight / lightPDF;
					}
				}

				//	BXDF
				f = event.mediumInteraction ? event.medium->phase->Sample_p(event.wo, &event.wi, *sampler) : hit.object->material->bxdf->Sample_f(event, *sampler, scatterPDF);
				//scatterPDF = event.mediumInteraction ? 

				r.o = hit.point;
				r.d = event.wi;

				Spectrum Tr(1);
				Medium *med = event.medium;
				scatterIntersect = _scene.IntersectTr(r, hit, *sampler, med, &Tr);



				/*
					Equiangular Generators
				*/
				Real equiangularT;
				const Spectrum trEquiangular = event.medium->SampleEquiangular(r, *sampler, event, lightPoint, &equiangularT, &equiangularPDF);
				const Vec3 equiangularPoint = r.o + r.d * equiangularT;
				distancePDF = event.medium->PDFDistance(equiangularT);

			}

			if (!event.mediumInteraction) {
				if (hit.object->material && hit.object->material->bxdf) {
					event.wo = -r.d;	//Compute wo before SurfaceLocalise()
					event.SurfaceLocalise();	//Calculate tangent space wo and wi

					Real lightDistPdf = 1;
					const Light *l = _scene.lightSampler->Sample(event, *sampler, &lightDistPdf);
					Spectrum Ld(0);
					Real scatteringPDF, lightPDF;
					Spectrum f, Li = l->Sample_Li(event, sampler, lightPDF);	//Solid angle PDF
					lightPDF *= lightDistPdf;	//True pdf of light
					if (lightPDF > 0 && !Li.IsBlack()) {	//Add light sample contribution
						f = hit.object->material->bxdf->f(event) * std::abs(event.wiL.y);
						scatteringPDF = hit.object->material->bxdf->Pdf(event.woL, event.wiL, event);
						if (!f.IsBlack() && scatteringPDF > 0) {
							const Real weight = PowerHeuristic(1, lightPDF, 1, scatteringPDF);
							Ld += Li * f * weight / lightPDF;
						}
					}
					f = hit.object->material->bxdf->Sample_f(event, *sampler, scatteringPDF);
					f *= std::abs(event.wiL.y);	//This must follow previous line to allow computation of wiL.

					r.o = hit.point;
					r.d = event.wi;
					event.medium = hit.object->material->mediaBoundary.GetMedium(event.wi, hit.normalG);	//Evaluate any medium we are going into before we get new hit

					scatterIntersect = _scene.Intersect(r, hit);	//Go to next path vertex and potential light contribution

					if (scatteringPDF > 0 && !f.IsBlack()) {	//Add bsdf-scattering light contribution. NOTE TO SELF: if we do bsdf sampling first, could use hit.point for light sampling
						if (const Light *nl = scatterIntersect ? hit.object->material->light : (Light *)_scene.envLight) {
							if (nl != l) lightDistPdf = _scene.lightSampler->Pdf(event, nl); //Recalculate light distribution pdf if we don't already know it
							lightPDF = lightDistPdf * nl->PDF_Li(event);
							Li = Spectrum(0);
							if (scatterIntersect) Li = nl->L(event);
							else Li = nl->Le(r);	//Special case for infinite lights
							const Real weight = PowerHeuristic(1, scatteringPDF, 1, lightPDF);
							if (!Li.IsBlack() && lightPDF > 0) Ld += Li * f * weight / scatteringPDF;
						}
					}
					else break;	//Don't continue path if bsdf is 0 or if scattering pdf is 0

					L += beta * Ld;
					beta *= f / scatteringPDF;
				}
				else {
					event.medium = hit.object->material->mediaBoundary.GetMedium(r.d, hit.normalG);	//Set medium for next ray
					r.o = hit.point + hit.normalG * (maths::Dot(event.hit->normalG, r.d) < 0 ? -SURFACE_EPSILON : SURFACE_EPSILON);
					if(bounces > 0) scatterIntersect = _scene.Intersect(r, hit);	//Go to next path vertex, but...
					bounces--;	//don't consider it a bounce (no scatter event)
					continue;
				}
			}
			else if(event.medium) {
				event.wo = -r.d;
				const Vec3 scatterPoint = hit.point;	//Keep the point in which the media scatter occurs so we can reuse hit.
				Real lightDistPdf = 1;
				const Light *l = _scene.lightSampler->Sample(event, *sampler, &lightDistPdf);
				Spectrum Ld(0);
				Real scatteringPDF, lightPDF;
				Spectrum p, Li = l->Sample_Li(event, sampler, lightPDF);
				lightPDF *= lightDistPdf;	//True pdf of light
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

				r.o = hit.point;
				r.d = event.wi;

				Spectrum Tr(1);
				Medium *med = event.medium;
				scatterIntersect = _scene.IntersectTr(r, hit, *sampler, med, &Tr);	//Direct lighting from phase scatter (skips through media)

				if (scatteringPDF > 0) {	//Add phase-scattering light contribution
					if (const Light *nl = scatterIntersect ? hit.object->material->light : (Light *)_scene.envLight) {
						if (nl != l) lightDistPdf = _scene.lightSampler->Pdf(event, nl); //Recalculate light distribution pdf if we don't already know it
						lightPDF = lightDistPdf * nl->PDF_Li(event);
						Li = Spectrum(0);
						if (scatterIntersect) Li = nl->L(event);
						else Li = nl->Le(r);	//Special case for infinite lights
						const Real weight = PowerHeuristic(1, scatteringPDF, 1, lightPDF);
						if (!Li.IsBlack() && lightPDF > 0) Ld += Li * Tr * weight / scatteringPDF;
					}
				}
 				else break;

				r.o = scatterPoint;
				r.d = event.wi;
				scatterIntersect = _scene.Intersect(r, hit);	//Next path vertex (doesn't skip through media)

				L += beta * Ld;
				//p is equal to the scattering pdf, so beta *= p / scatteringPDF is redundant
			}

			if (bounces > minBounces) {
				const Real q = std::max((Real).05, 1 - beta.y());
				if (sampler->Get1D() < q) break;
				beta /= (Real)1 - q;
			}
		}
		else {
			if (bounces == 0) L += ((Light*)_scene.envLight)->Le(r) * beta; //Beta is not always 1 here (due to vol attenuation)
			break;
		}
	}
	return L;
}

LAMBDA_END