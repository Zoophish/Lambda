#pragma once
#include "TextureAdapter.h"
#include "SurfaceScatterEvent.h"
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
			return f(_event);
		}

		virtual Spectrum Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const {
			return Spectrum(0);
		}

		inline Real CosineHemispherePdf(const Vec3 &_wo, const Vec3 &_wi) const {
			return SameHemisphere(_wo, _wi) ? std::abs(_wi.y) * INV_PI : 0;
		}

		inline bool SameHemisphere(const Vec3 &_w1, const Vec3 &_w2) const {
			return _w1.y * _w2.y > 0;
		}
		
};

//----	Utility functions:	---- TO DO - NEEDS OPTIMISING AND MOVING
inline Real CosTheta(const Vec3 &_w) { return _w.y; }

inline Real Cos2Theta(const Vec3 &_w) { return _w.y * _w.y; }

inline Real Sin2Theta(const Vec3 &_w) { return std::max((Real)0, 1 - _w.y * _w.y); }

inline Real SinTheta(const Vec3 &_w) { return std::sqrt(Sin2Theta(_w)); }

inline Real CosPhi(const Vec3 &_w) {
	const Real sinTheta = SinTheta(_w);
	return (sinTheta == 0) ? 0 : maths::Clamp(_w.x / sinTheta, (Real)-1, (Real)1);
}

inline Real Cos2Phi(const Vec3 &_w) {
	return CosPhi(_w) * CosPhi(_w);
}

inline Real SinPhi(const Vec3 &_w) {
	const Real sinTheta = SinTheta(_w);
	return (sinTheta == 0) ? 0 : maths::Clamp(_w.y / sinTheta, (Real)-1, (Real)1);
}

inline Real Sin2Phi(const Vec3 &_w) {
	return SinPhi(_w) * SinPhi(_w);
}

inline Real Tan2Theta(const Vec3 &_w) {
	return Sin2Theta(_w) / Cos2Theta(_w);
}

inline Real TanTheta(const Vec3 &_w) {
	return SinTheta(_w) / CosTheta(_w);
}

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