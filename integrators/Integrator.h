#pragma once
#include <sampling/Sampler.h>
#include <core/Scene.h>
class Ray;

class Integrator {
	public:
		Sampler *sampler;

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
			if (lightPDF > 0 && !Li.IsBlack()) {
				const Spectrum f = _event.hit->object->bxdf->f(_event) * std::abs(maths::Dot(_event.hit->normalS, _event.wi));
				const Vec3 woL = _event.ToLocal(_event.wo), wiL = _event.ToLocal(_event.wi);
				scatteringPDF = _event.hit->object->bxdf->CosineHemispherePdf(woL, wiL);
				if (!f.IsBlack()) {
					const Real weight = PowerHeuristic(1, lightPDF, 1, scatteringPDF);
					Ld += Li * f * weight / lightPDF;
				}
			}
			Spectrum f = _event.hit->object->bxdf->Sample_f(_event, sampler->Get2D(), scatteringPDF);
			f *= std::abs(maths::Dot(_event.wi, _event.hit->normalS));
			if (scatteringPDF > 0 && !f.IsBlack()) {
				Real weight = 1;
				Spectrum Li(0);
				lightPDF = _light.PDF_Li(_event);
				if (lightPDF == 0) return Ld;
				weight = PowerHeuristic(1, scatteringPDF, 1, lightPDF);
				SurfaceScatterEvent lightIntersect;
				RayHit lightHit;
				lightIntersect.hit = &lightHit;
				lightIntersect.wo = _event.wi;
				const Ray r(_event.hit->point + _event.hit->normalG * .00001, _event.wi);
				if (_scene.Intersect(r, lightHit)) {
					if (lightIntersect.hit->object->light == &_light)
						Li = lightIntersect.hit->object->light->L(lightIntersect);
				}
				else {
					Li = _light.Le(r);
				}
				if (!Li.IsBlack()) Ld += f * Li * weight / scatteringPDF;
			}
			return Ld;
		}

	protected:
		inline Real PowerHeuristic(int nf, Real fPdf, int ng, Real gPdf) const {
			Real f = nf * fPdf, g = ng * gPdf;
			return (f * f) / (f * f + g * g);
		}
};