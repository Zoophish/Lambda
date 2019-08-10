#pragma once
#include "BxDF.h"

class SpecularReflectionBSDF : public BxDF {
	public:
		Fresnel *fresnel;
		Spectrum albedo;
	
		SpecularReflectionBSDF(const Spectrum &_albedo, Fresnel *_fresnel) : BxDF((BxDFType)(BSDF_REFLECTION | BSDF_SPECULAR)) {
			fresnel = _fresnel;
			albedo = _albedo;
		}
	
		Spectrum f(const Vec3 &_wo, const Vec3 &_wi) const override {
			return Spectrum(0);
		}
	
		Spectrum Sample_f(const Vec3 &_wo, Vec3 *_wi, const Real _eta, const Vec2 &_smpl, Real *_pdf, BxDFType *_type = nullptr) const override {
			*_pdf = 1;
			*_wi = -_wo;
			const Real cosTheta = _wi->z;
			return fresnel->Evaluate(cosTheta, _eta) * albedo / std::abs(cosTheta);
		}
};

class SpecularTransmissionBSDF : public BxDF {
	public:
		Spectrum 
};