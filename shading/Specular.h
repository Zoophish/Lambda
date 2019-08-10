#pragma once
#include "BxDF.h"

class SpecularBRDF : public BxDF {
	public:
		Fresnel *fresnel;
		Spectrum albedo;
	
		SpecularBRDF(const Spectrum &_albedo, Fresnel *_fresnel) : BxDF((BxDFType)(BSDF_REFLECTION | BSDF_SPECULAR)) {
			fresnel = _fresnel;
			albedo = _albedo;
		}
	
		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return Spectrum(0);
		}
	
		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_smpl, BxDFType *_type = nullptr) const override {
			_event.pdf = 1;
			*_event.wi = -*_event.wo;
			const Real cosTheta = _event.wi->z;
			return fresnel->Evaluate(cosTheta, _event.eta) * albedo / std::abs(cosTheta);
		}
};

class SpecularBTDF : public BxDF {
	public:
		FresnelDielectric fresnel;
		Spectrum albedo;

		SpecularBTDF(const Spectrum &_albedo, const Real _etaT) : BxDF((BxDFType)(BSDF_TRANSMISSION | BSDF_SPECULAR)) {
			albedo = _albedo;
			fresnel.etaT = _etaT;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return Spectrum(0);
		}

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_smpl, BxDFType *_type = nullptr) const override {
			_event.pdf = 1;
			const bool outside = _event.wo->z > 0.;
			const Real etaI = outside ? fresnel.etaT : _event.eta;
			const Real etaT = outside ? _event.eta : fresnel.etaT;
			*_event.wi = Refract(_event, etaI / etaT);
			const Spectrum trans = albedo * (Spectrum(1) - fresnel.Evaluate(_event.wi->z, etaI));

			const Real inv = (Real)1 / std::abs(_event.wi->z);
			return trans * inv;
		}

	protected:
		inline Vec3 Refract(const SurfaceScatterEvent &_event, const Real etaD) const {
			const Real cosTheta = maths::Dot(*_event.wi, *_event.n);
			const Real sin2Theta = std::max((Real)0, (Real)1 - cosTheta * cosTheta);
			const Real sin2ThetaT = etaD * etaD * sin2Theta;
			if (sin2ThetaT >= 1) { //TIR
				return -*_event.wo;
			}
			const Real cosThetaT = std::sqrt(1 - sin2ThetaT);
			return -*_event.wi * etaD + *_event.n * (etaD * cosTheta - cosThetaT);
		}
};