#pragma once
#include "../TextureAdapter.h"
#include "../SurfaceScatterEvent.h"
#include <sampling/Sampling.h>

#define SURFACE_EPSILON 1e-5

class BxDF {
	public:
		enum BxDFType {
			BxDF_REFLECTION = BITFLAG(0),
			BxDF_TRANSMISSION = BITFLAG(1),
			BxDF_DIFFUSE = BITFLAG(2),
			BxDF_GLOSSY = BITFLAG(3),
			BxDF_SPECULAR = BITFLAG(4),
		};

		const BxDFType type;

		BxDF(const BxDFType _type);

		virtual Spectrum f(const SurfaceScatterEvent &_event) const = 0;

		virtual Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const;

		virtual Spectrum Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const;

		virtual Real Pdf(const Vec3 &_wo, const Vec3 &_wi) const;

		static inline Real CosineHemispherePdf(const Vec3 &_wo, const Vec3 &_wi) {
			return SameHemisphere(_wo, _wi) ? std::abs(_wi.y) * INV_PI : 0;
		}
};



class LambertianBRDF : public BxDF {
	public:
		TextureAdapter albedo;

		LambertianBRDF(Texture *_albedo = nullptr);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const override;
};



class MixBSDF : public BxDF {
	public:
	BxDF *a, *b;
	Real factor;

	MixBSDF(BxDF *_a, BxDF *_b, const Real _factor = .5);

	Spectrum f(const SurfaceScatterEvent &_event) const override;
};