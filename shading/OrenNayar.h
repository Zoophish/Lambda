#pragma once
#include "BxDF.h"

class OrenNayarBRDF : public BxDF {
	public:
		TextureAdapter albedo;

		inline void SetSigma(const Real _sigma) {
			const Real sigma2 = _sigma * _sigma;
			A = (Real)1 - sigma2 / ((Real)2 * (sigma2 + (Real).33));
			B = ((Real).45 * sigma2) / (sigma2 + (Real).09);
		}

		OrenNayarBRDF(Texture *_albedo = nullptr, const Real _sigma = .5) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_DIFFUSE)) {
			albedo.SetTexture(_albedo);
			SetSigma(_sigma);
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			const Vec3 wi = _event.ToLocal(_event.wi);
			const Vec3 wo = _event.ToLocal(_event.wo);
			const Real thetaI = std::acos(wi.y);
			const Real thetaO = std::acos(wo.y);
			const Real alpha = std::max(thetaI, thetaO);
			const Real beta = std::min(thetaI, thetaO);
			const Real phiI = std::acos(BxDF::CosPhi(wi));
			const Real phiO = std::acos(BxDF::CosPhi(wo));
			return albedo.GetUV(_event.hit->uvCoords) * INV_PI * (A + B * std::max((Real)0, std::cos(phiI - phiO)) * std::sin(alpha) * std::tan(beta));
		}

	protected:
		Real A, B;
};