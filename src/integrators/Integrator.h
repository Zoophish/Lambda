#pragma once
#include <sampling/Sampler.h>
#include <core/Scene.h>
#include <shading/surface/BxDF.h>
#include <shading/media/Media.h>

LAMBDA_BEGIN

class Integrator {
	public:
		Sampler *sampler;

		virtual Integrator *clone() const = 0;

		virtual Spectrum Li(Ray _ray, const Scene &_scene) const = 0;

		Spectrum SampleOneLight(ScatterEvent &_event, const Scene &_scene) const;

		Spectrum EstimateDirect(ScatterEvent &_event, const Scene &_scene, const Light &_light) const;

	protected:
		static inline Real PowerHeuristic(int nf, Real fPdf, int ng, Real gPdf) {
			const Real f = nf * fPdf, g = ng * gPdf;
			return (f * f) / (f * f + g * g);
		}
};

LAMBDA_END