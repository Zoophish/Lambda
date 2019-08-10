#pragma once
#include "BxDF.h"
#include "SurfaceScatterEvent.h"

class LambertianBSDF : public BxDF {
	public:
		Spectrum albedo;

		LambertianBSDF(const Spectrum &_albedo) : BxDF((BxDFType)(BSDF_REFLECTION | BSDF_DIFFUSE)) {
			albedo = _albedo;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {

		}
};