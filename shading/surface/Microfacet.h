#pragma once
#include "../Fresnel.h"
#include "../MicrofacetDistribution.h"

class MicrofacetBRDF : public BxDF {
	public:
		Real etaT = .0001;
		TextureAdapter albedo;
		MicrofacetDistribution *distribution;
		Fresnel *fresnel;


		MicrofacetBRDF(Texture *_albedo, MicrofacetDistribution *_distribution, Fresnel *_fresnel);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi) const override;
};

//class MicrofacetBSDF {};