#pragma once
#include <sampling/Sampler.h>
#include <core/Scene.h>
#include <shading/surface/BxDF.h>
#include <shading/media/Media.h>

class Integrator {
	public:
		Sampler *sampler;

		virtual Integrator *clone() const = 0;

		virtual Spectrum Li(const Ray &_ray, const Scene &_scene) const = 0;

		Spectrum SampleOneLight(SurfaceScatterEvent &_event, const Scene &_scene) const {
			if (_event.hit->object->bxdf) {
				Real lightPdf = 1;
				Light *l = _scene.lights[_scene.lightDistribution.SampleDiscrete(sampler->Get1D(), &lightPdf)];
				return EstimateDirect(_event, _scene, *l) / lightPdf;
			}
			return Spectrum(0);
		}

		Spectrum EstimateDirect(SurfaceScatterEvent &_event, const Scene &_scene, const Light &_light) const {
			Spectrum Ld(0);
			Real scatteringPDF, lightPDF;
			Spectrum Li = _light.Sample_Li(_event, sampler, lightPDF);
			Medium *medium = nullptr;
			if(_event.hit->object->mediaBoundary) medium = _event.hit->object->mediaBoundary->GetMedium(_event.wo, _event.hit->normalG);
			if (lightPDF > 0 && !Li.IsBlack()) {
				Spectrum f;
				if (_event.hit->object->bxdf) {
					f = _event.hit->object->bxdf->f(_event) * std::abs(_event.wiL.y);
					scatteringPDF = _event.hit->object->bxdf->Pdf(_event.woL, _event.wiL);
					
				}
				else if (medium) {
					Real p = medium->p(_event.wo, _event.wi);
					f = Spectrum(p);
					scatteringPDF = p;
				}
				if (!f.IsBlack() && scatteringPDF > 0) {
					const Real weight = PowerHeuristic(1, lightPDF, 1, scatteringPDF);
					Ld += Li * f * weight / lightPDF;
				}
			}
			Spectrum f = _event.hit->object->bxdf->Sample_f(_event, sampler->Get2D(), scatteringPDF);
			f *= std::abs(_event.wiL.y);
			if (scatteringPDF > 0 && !f.IsBlack()) {
				Real weight = 1;
				Spectrum Li(0);
				lightPDF = _light.PDF_Li(_event);
				if (lightPDF == 0) return Ld;
				weight = PowerHeuristic(1, scatteringPDF, 1, lightPDF);
				SurfaceScatterEvent bsdfIntersect;
				bsdfIntersect.scene = &_scene;
				RayHit lightHit;
				bsdfIntersect.hit = &lightHit;
				bsdfIntersect.wo = -_event.wi;
				Ray r(_event.hit->point + _event.hit->normalG * .00001, _event.wi);
				Spectrum tr(1);
				//const bool intersect = IntersectTr(_scene, r, lightHit, &tr);
				if (_scene.Intersect(r, lightHit)) {
				//if(intersect) {
					if (bsdfIntersect.hit->object->light == &_light)
						Li = bsdfIntersect.hit->object->light->L(bsdfIntersect);
				}
				else {
					Li = _light.Le(r);
				}
				if (!Li.IsBlack()) Ld += f * Li * weight / scatteringPDF;
			}
			return Ld;
		}

		bool IntersectTr(const Scene &_scene, Ray &_ray, RayHit &_hit, Spectrum *_tr) const {
			*_tr = Spectrum(1.f);
			while (true) {
				bool hitSurface = _scene.Intersect(_ray, _hit);
				if (_hit.object->mediaBoundary && _hit.object->mediaBoundary->GetMedium(_ray.d, _hit.normalS))
					*_tr *= _hit.object->mediaBoundary->GetMedium(_ray.d, _hit.normalS)->Tr(_ray, _hit.tFar, *sampler);
				if (!hitSurface)
					return false;
				if (_hit.object->bxdf)
					return true;
				_ray.o = _hit.point + _ray.d * .0001;
			}
		}

	protected:
		inline Real PowerHeuristic(int nf, Real fPdf, int ng, Real gPdf) const {
			Real f = nf * fPdf, g = ng * gPdf;
			return (f * f) / (f * f + g * g);
		}
};