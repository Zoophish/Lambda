#pragma once
#include "BxDF.h"
#include "SurfaceScatterEvent.h"

class LambertianBRDF : public BxDF {
	public:
		TextureAdapter albedo;

		LambertianBRDF(Texture *_albedo) : BxDF((BxDFType)(BSDF_REFLECTION | BSDF_DIFFUSE)) {
			albedo.texture = _albedo;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return albedo.GetUV(*_event.uv) * INV_PI;
		}

		Spectrum Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const override {
			return albedo.GetUV(*_event.uv);
		}
};