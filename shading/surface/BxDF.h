#pragma once
#include "../TextureAdapter.h"
#include "../SurfaceScatterEvent.h"
#include <sampling/Sampling.h>

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

		BxDF(const BxDFType _type) : type(_type) {}

		virtual Spectrum f(const SurfaceScatterEvent &_event) const = 0;

		virtual Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const {
			_event.wiL = Sampling::SampleCosineHemisphere(_u);
			if (_event.woL.y < 0) _event.wiL.y *= -1;
			_pdf = CosineHemispherePdf(_event.woL, _event.wiL);
			_event.wi = _event.ToWorld(_event.wiL);
			_event.hit->point += _event.hit->normalG * .001;
			return f(_event);
		}

		virtual Spectrum Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const {
			return Spectrum(0);
		}

		virtual Real Pdf(const Vec3 &_wo, const Vec3 &_wi) const {
			return SameHemisphere(_wo, _wi) ? std::abs(_wi.y) * INV_PI : 0;
		}

		inline Real CosineHemispherePdf(const Vec3 &_wo, const Vec3 &_wi) const {
			return SameHemisphere(_wo, _wi) ? std::abs(_wi.y) * INV_PI : 0;
		}
};

class ScaledBxDF : public BxDF {
	public:
		BxDF* bxdf;
		Spectrum scale;

		ScaledBxDF(BxDF *_bxdf, const Spectrum &_scale) : BxDF(bxdf->type) {
			bxdf = _bxdf;
			scale = _scale;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return scale * bxdf->f(_event);
		}
};