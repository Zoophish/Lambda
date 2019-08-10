#pragma once
#include "BxDF.h"
#include "SurfaceScatterEvent.h"

class LambertianBRDF : public BxDF {
	public:
		Spectrum albedo;

		LambertianBRDF(const Spectrum &_albedo) : BxDF((BxDFType)(BSDF_REFLECTION | BSDF_DIFFUSE)) {
			albedo = _albedo;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return albedo * INV_PI;
		}

		Spectrum Rho(const Vec3 &_w, const unsigned _nSample, Vec2 *_smpls) const override {
			return albedo;
		}
};