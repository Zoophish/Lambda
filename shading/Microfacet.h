#pragma once
#include "Fresnel.h"
#include "MicrofacetDistribution.h"

class MicrofacetBRDF : public BxDF {
	public:
		Real etaT;
		TextureAdapter albedo;
		MicrofacetDistribution *distribution;
		Fresnel *fresnel;


		MicrofacetBRDF(Texture *_albedo, MicrofacetDistribution *_distribution, Fresnel *_fresnel) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_GLOSSY)) {
			albedo.SetTexture(_albedo);
			distribution = _distribution;
			fresnel = _fresnel;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			const Real cosThetaO = std::abs(_event.woL.y), cosThetaI = std::abs(_event.wiL.y);
			const Vec3 wh = (_event.wiL + _event.woL).Normalised();
			if (cosThetaI == 0 || cosThetaO == 0) return Spectrum(0.);
			if (wh.x == 0 && wh.y == 0 && wh.z == 0) return Spectrum(0.);
			const Spectrum fr = fresnel->Evaluate(maths::Dot(_event.wiL, wh), etaT);
			return albedo.GetUV(_event.hit->uvCoords) * distribution->D(wh) * distribution->G(_event.woL, _event.wiL) * fr /
				(4 * cosThetaI * cosThetaO);
		}
};