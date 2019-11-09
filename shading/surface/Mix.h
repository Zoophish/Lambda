#pragma once
#include "BxDF.h"

class MixBSDF : public BxDF {
	public:
		BxDF *a, *b;
		Real factor;

		MixBSDF(BxDF *_a, BxDF *_b, const Real _factor = .5) : BxDF(BxDF_DIFFUSE) {
			a = _a;
			b = _b;
			factor = _factor;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return a->f(_event) * ((Real)1 - factor) + b->f(_event) * factor;
		}
};