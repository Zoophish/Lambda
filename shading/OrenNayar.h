#pragma once
#include "BxDF.h"
#include "SurfaceScatterEvent.h"

//REPLACE albedo with pointer to texture.

class OrenNayarBRDF : public BxDF {
	public:
		Texture *albedo;

		inline void SetSigma(const Real _sigma) {
			const Real sigma2 = _sigma * _sigma;
			A = (Real)1 - sigma2 / ((Real)2 * (sigma2 + (Real).33));
			B = ((Real).45 * sigma2) / (sigma2 + (Real).09);
		}

		OrenNayarBRDF(Texture *_albedo, const Real _sigma) : BxDF((BxDFType)(BSDF_REFLECTION | BSDF_DIFFUSE)) {
			albedo = _albedo;
			SetSigma(_sigma);
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			const Real thetaI = std::acos(_event.wi->z);
			const Real thetaO = std::acos(_event.wo->z);
			const Real alpha = std::max(thetaI, thetaO);
			const Real beta = std::min(thetaI, thetaO);
			const Real phiI = std::acos(BxDF::CosPhi(*_event.wi));
			const Real phiO = std::acos(BxDF::CosPhi(*_event.wo));
			return albedo * INV_PI * (A + B * std::max((Real)0, std::cos(phiI - phiO)) * std::sin(alpha) * std::tan(beta));
		}

	protected:
		Real A, B;
};