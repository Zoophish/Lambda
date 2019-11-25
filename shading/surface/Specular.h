#pragma once
#include "BxDF.h"
#include "../Fresnel.h"

class FresnelBSDF : public BxDF {
	public:
		Real ior;
		TextureAdapter albedo;

		FresnelBSDF(Texture *_albedo, const Real _ior);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const override;
};

class SpecularBRDF : public BxDF {
	public:
		Fresnel *fresnel;
		TextureAdapter albedo;
	
		SpecularBRDF(Texture *_albedo, Fresnel *_fresnel) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_SPECULAR)) {
			fresnel = _fresnel;
			albedo.SetTexture(_albedo);
		}
	
		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return Spectrum(0);
		}
		
		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override {
			_event.pdf = 1;
			_pdf = 1;
			_event.wiL = Vec3(-_event.woL.x, _event.woL.y, -_event.woL.z);
			const Real cosTheta = _event.wiL.y;
			_event.wi = _event.ToWorld(_event.wiL);
			return albedo.GetUV(_event.hit->uvCoords) / std::abs(cosTheta);
		}
};

class SpecularBTDF : public BxDF {
	public:
		FresnelDielectric fresnel;
		TextureAdapter albedo;

		SpecularBTDF(Texture *_albedo, const Real _etaT);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const override;
};