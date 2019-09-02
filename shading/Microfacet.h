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
			const Vec3 wh = (_event.wiL + _event.woL) * .5;
			if (cosThetaI == 0 || cosThetaO == 0) return Spectrum(0);
			if (wh.x == 0 && wh.y == 0 && wh.z == 0) return Spectrum(0);
			const Spectrum F = fresnel->Evaluate(maths::Dot(_event.wiL, wh), etaT);
			return albedo.GetUV(_event.hit->uvCoords) * distribution->D(wh) * distribution->G(_event.woL, _event.wiL) * F /
				(4 * cosThetaI * cosThetaO);
		}

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override {
			const Vec3 wh = distribution->Sample_wh(_u, _event.woL);
			_event.wiL = Reflect(_event.woL, wh).Normalised();
			if (!SameHemisphere(_event.woL, _event.wiL)) {
				_pdf = 0;
				return Spectrum(0);
			}
			_pdf = distribution->Pdf(wh) / (4 * maths::Dot(_event.woL, wh));
			_event.wi = _event.ToWorld(_event.wiL);
			_event.hit->point += _event.hit->normalG * .0001;
			return f(_event);
		}

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi) const override {
			if (!SameHemisphere(_wo, _wi)) return 0;
			const Vec3 wh = (_wo + _wi).Normalised();
			return distribution->Pdf(wh) / (4 * maths::Dot(_wo, wh));
		}
};

class MicrofacetBTDF : public BxDF {

};