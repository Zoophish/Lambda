#include "MISVolumetricPathIntegrator.h"


LAMBDA_BEGIN

MISVolumetricPathIntegrator::MISVolumetricPathIntegrator(Sampler *_sampler, const unsigned _maxBounces, const unsigned _minBounces) {
	maxBounces = _maxBounces;
	minBounces = _minBounces;
	sampler = _sampler;
}

Integrator *MISVolumetricPathIntegrator::clone() const {
	return new MISVolumetricPathIntegrator(*this);
}

static Medium *InMedium(const Vec3 &_p, const Scene &_scene) {
	//Embree rtcCollide... what medium is the camera in?
	return nullptr;
}

// TO SELF: Real *_f might be redundant
Spectrum MISVolumetricPathIntegrator::LdMediumPoint(ScatterEvent &_event, const Vec3 &_p, const Light &_l, Real _lpdf, Real *_f) const {
	const Scene &scene = *_event.scene;
	RayHit &hit = *_event.hit;

	const Vec3 scatterPoint = hit.point;	//Keep the point in which the media scatter occurs so we can reuse hit.
	hit.point = _p;

	Real scatteringPDF, lightPDF;
	Spectrum p, Li = _l.Sample_Li(_event, sampler, lightPDF);
	lightPDF *= _lpdf;	//Final pdf of light & point on light

	Spectrum Ld(0);
	if (lightPDF > 0 && !Li.IsBlack()) {
		scatteringPDF = _event.medium->phase->p(_event.wo, _event.wi);
		p = Spectrum(scatteringPDF);
		if (scatteringPDF > 0) {
			const Real weight = PowerHeuristic(1, lightPDF, 1, scatteringPDF);
			Ld += Li * p * weight / lightPDF;
		}
	}

	scatteringPDF = _event.medium->phase->Sample_p(_event.wo, &_event.wi, *sampler);
	*_f = scatteringPDF;

	RayHit rhit;
	Ray r = { _p, _event.wi };

	Spectrum Tr(1);
	Medium *med = _event.medium;
	const bool scatterIntersect = _event.scene->IntersectTr(r, rhit, *sampler, med, &Tr);	//Direct lighting from phase scatter (skips through media)
	
	if (scatteringPDF > 0) {	//Add phase-scattering light contribution
		if (const Light *nl = scatterIntersect ? rhit.object->material->light : (Light *)scene.envLight) {
			if (nl != &_l) {
				_lpdf = scene.lightSampler->Pdf(_event, nl); //Recalculate light distribution pdf if don't already know it
			}
			lightPDF = _lpdf * nl->PDF_Li(_event);
			Li = Spectrum(0);
			if (scatterIntersect) Li = nl->L(_event);
			else Li = nl->Le(r);	//Special case for infinite lights
			const Real weight = PowerHeuristic(scatteringPDF, lightPDF);
			if (!Li.IsBlack() && lightPDF > 0) {
				Ld += Li * Tr * weight / scatteringPDF;
			}
		}
	}

	hit.point = scatterPoint;
	return Ld;
}

Spectrum MISVolumetricPathIntegrator::Li(Ray r, const Scene &_scene) const {
	Spectrum L(0), beta(1);
	RayHit hit;
	ScatterEvent event;
	event.hit = &hit;
	event.scene = &_scene;
	event.wo = -r.d;
	bool scatterIntersect = false;
	event.medium = InMedium(r.o, _scene);
	for (int bounces = 0; bounces < maxBounces; ++bounces) {
		if (bounces == 0 ? _scene.Intersect(r, hit) : scatterIntersect) {

			if (bounces == 0) {	//Direct lighting on bounce 0 done here
				if (hit.object->material->light) {
					if (event.medium) {
						L += hit.object->material->light->L(event) * event.medium->Tr(r, hit.tFar, *sampler);
					}
					else L += hit.object->material->light->L(event); //Beta is always 1 here, so it is excluded from the product
				}
			}

			Real distanceT, distancePDF;
			//bool mediumPath = sampler->Get1D() > 0.5;
			if (event.medium) {
				beta *= event.medium->SampleDistance(r, *sampler, event, &distanceT, &distancePDF);
				//else beta *= event.medium->SampleEquiangular(r, *sampler, event, &distanceT, &distancePDF);
			}
			else event.mediumInteraction = false;

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
							const Real weight = PowerHeuristic(lightPDF, scatteringPDF);
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
				else {	// make next event a medium interaction
					event.medium = hit.object->material->mediaBoundary.GetMedium(r.d, hit.normalG);	//Set medium for next ray
					r.o = hit.point + hit.normalG * (maths::Dot(event.hit->normalG, r.d) < 0 ? -SURFACE_EPSILON : SURFACE_EPSILON);
					if(bounces > 0) scatterIntersect = _scene.Intersect(r, hit);	//Go to next path vertex, but...
					bounces--;	//don't consider it a bounce (no scatter event)
					continue;
				}
			}
			else if (event.medium) {
				event.wo = -r.d;

				// TO SELF: Sample ray segment from light sampler rather than point
				Real lightPDF;
				const Light *l = _scene.lightSampler->Sample(event, *sampler, &lightPDF);
				Real lightPointPdf;
				const Vec3 lightPoint = l->SamplePoint(*sampler, event, &lightPointPdf);
				//lightPDF *= lightPointPdf;

				Spectrum Ld(0);

				// Distance Generators
				Real equiangularT, distancePDF2, equiangularPDF, equiangularPDF2;
				//const Spectrum trDistance = event.medium->SampleDistance(r, *sampler, event, &distanceT, &distancePDF);
				const Vec3 distancePoint = r.o + r.d * distanceT;
				equiangularPDF2 = event.medium->PDFEquiangular(r, lightPoint, event.hit->tFar, distanceT);
				const Real distanceW = PowerHeuristic(distancePDF, equiangularPDF2);

				const Spectrum trEquiangular = event.medium->SampleEquiangular(r, *sampler, event, lightPoint, &equiangularT, &equiangularPDF);
				const Vec3 equiangularPoint = r.o + r.d * equiangularT;
				distancePDF2 = event.medium->PDFDistance(equiangularT);
				const Real equiangularW = PowerHeuristic(equiangularPDF, distancePDF2);

				// Decide what path to continue (last function to make modifications)
				Real phasePDF;
				//if (sampler->Get1D() > 0.5) {
				//	const Spectrum liDistance = LdMediumPoint(event, distancePoint, *l, &f, &r);
				//	Ld += liDistance * distanceW / distancePDF;
				//	const Spectrum liEquiangular = LdMediumPoint(event, equiangularPoint, *l, &f, &r);
				//	Ld += liEquiangular * equiangularW / equiangularPDF;
				//	r.o = equiangularPoint;
				//}
				//else {
					//const Spectrum liEquiangular = LdMediumPoint(event, equiangularPoint, *l, &f, &r);
					//Ld += liEquiangular * equiangularW / equiangularPDF;
					const Spectrum liDistance = LdMediumPoint(event, distancePoint, *l, lightPDF, &phasePDF);
					Ld += liDistance * 1 / distancePDF;
					r.o = distancePoint;
				//}

				r.d = event.wi;
				//event.medium = hit.object->material->mediaBoundary.GetMedium(event.wi, hit.normalG);
				scatterIntersect = _scene.Intersect(r, hit);	//Next path vertex (doesn't skip through media)

				L += beta * Ld;
				// phase function is perfectly proportional to pdf => beta *= p / p is redundant
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