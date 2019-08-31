#pragma once
#include "Fresnel.h"
#include "MicrofacetDistribution.h"

class MicrofacetBRDF : public BxDF {
	public:
		MicrofacetBRDF(Texture *_albedo, MicrofacetDistribution *_distribution, Fresnel *_fresnel) {
			albedo.SetTexture(_albedo);
			distribution = _distribution;
			fresnel = _fresnel;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			
			const Vec3 wh = (_event.wi + _event.wo).Normalised();

		}

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override {
			
		}

	protected:
		TextureAdapter albedo;
		MicrofacetDistribution *distribution;
		Fresnel *fresnel;
};