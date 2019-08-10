#pragma once
#include <image/Texture.h>
#include <core/Spectrum.h>
#include "SurfaceScatterEvent.h"

class BxDF {
	public:
		enum BxDFType {
			BSDF_REFLECTION = BITFLAG(0),
			BSDF_TRANSMISSION = BITFLAG(1),
			BSDF_DIFFUSE = BITFLAG(2),
			BSDF_GLOSSY = BITFLAG(3),
			BSDF_SPECULAR = BITFLAG(4),
			BSDF_ALL = 31
		};

		const BxDFType type;

		BxDF(const BxDFType _type) : type(_type) {}

		virtual Spectrum f(const SurfaceScatterEvent &_event) const = 0;

		virtual Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_smpl, BxDFType *_type = nullptr) const;

		virtual Spectrum Rho(const Vec3 &_w, const unsigned _nSample, Vec2* _smpls) const;

		//----	Surface-space utility functions:	----
		inline Real CosTheta(const Vec3 &_w) const { return _w.z; }
		inline Real Sin2Theta(const Vec3 &_w) const { return std::max((Real)0, 1 - _w.z * _w.z); }
		inline Real SinTheta(const Vec3 &_w) const { return std::sqrt(Sin2Theta(_w)); }
		inline Real CosPhi(const Vec3 &_w) const {
			const Real sinTheta = SinTheta(_w);
			return (sinTheta == 0) ? 0 : maths::Clamp(_w.x / sinTheta, (Real)-1, (Real)1);
		}
		inline Real SinPhi(const Vec3 &_w) const {
			const Real sinTheta = SinTheta(_w);
			return (sinTheta == 0) ? 0 : maths::Clamp(_w.y / sinTheta, (Real)-1, (Real)1);
		}
		inline Vec3 SampleCosHemisphere(const Real _r1, const Real _r2, Real *_pdf) const {
				const Real r = std::sqrt(_r1);
				const Real theta = PI2 * _r2;
				const Real cosTheta = std::cos(theta);
				*_pdf = cosTheta * INV_PI;
				return Vec3(r * cosTheta, r * std::sin(theta), std::sqrt(1.f - _r1));
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

class Fresnel {
	public:
		virtual Spectrum Evaluate(const Real _cosThetaI, const Real _etaT) const = 0;

		static Real FrDielectric(Real _cosThetaI, Real _etaI, Real _etaT) {
			_cosThetaI = maths::Clamp(_cosThetaI, (Real)-1, (Real)1);
			if (_cosThetaI < 0.) {
				std::swap(_etaI, _etaT);
				_cosThetaI = std::abs(_cosThetaI);
			}
			const Real sinThetaI = std::sqrt(std::max((Real)0, 1 - _cosThetaI * _cosThetaI));
			const Real sinThetaT = _etaI / _etaT * sinThetaI;
			if (sinThetaT >= 1) {
				return 1;
			}
			const Real cosThetaT = std::sqrt(std::max((Real)0, 1 - sinThetaT * sinThetaT));
			const Real rPara = ((_etaT * _cosThetaI) - (_etaI * cosThetaT)) / ((_etaT * _cosThetaI) + (_etaI * cosThetaT));
			const Real rPerp = ((_etaI * _cosThetaI) - (_etaT * cosThetaT)) / ((_etaI * _cosThetaI) + (_etaT * cosThetaT));
			return (rPara * rPara + rPerp * rPerp) * .5;
		}

		//Note to self: Could do with optimising.
		static Spectrum FrConductor(Real _cosThetaI, const Spectrum& _etaI, const Spectrum& _etaT, const Spectrum& _k) {
			_cosThetaI = maths::Clamp(_cosThetaI, (Real)-1, (Real)1);
			const Spectrum eta = _etaT / _etaI;
			const Spectrum etak = _k / _etaI;

			const Real cosThetaI2 = _cosThetaI * _cosThetaI;
			const Real sinThetaI2 = (Real)1 - cosThetaI2;
			const Spectrum eta2 = eta * eta;
			const Spectrum etak2 = etak * etak;

			const Spectrum t0 = eta2 - etak2 - sinThetaI2;
			const Spectrum a2plusb2 = Spectrum::Sqrt(t0 * t0 + eta2 * (Real)4 * etak2);
			const Spectrum t1 = a2plusb2 + cosThetaI2;
			const Spectrum a = Spectrum::Sqrt((a2plusb2 + t0) * (Real).5);
			const Spectrum t2 = a * _cosThetaI * (Real)2;
			const Spectrum rS = (t1 - t2) / (t1 + t2);

			const Spectrum t3 = a2plusb2 * cosThetaI2 + sinThetaI2 * sinThetaI2;
			const Spectrum t4 = t2 * sinThetaI2;
			const Spectrum rP = rS * (t3 - t4) / (t3 + t4);

			return (rP + rS) * (Real).5;
		}
};

class FresnelDielectric : public Fresnel {
	public:
		Real etaT;

		FresnelDielectric() {}

		FresnelDielectric(const Real _etaT) {
			etaT = _etaT;
		}

		Spectrum Evaluate(const Real _cosThetaI, const Real _etaI) const override {
			return Fresnel::FrDielectric(_cosThetaI, etaT, _etaI);
		}
};

class FresnelConductor : public Fresnel {
	public:
		Spectrum etaI, k;

		FresnelConductor(const Spectrum &_etaI, const Spectrum &_k) {
			etaI = _etaI;
			k = _k;
		}

		Spectrum Evaluate(const Real _cosThetaI, const Real _etaT) const override {
			return FrConductor(_cosThetaI, etaI, _etaT, k);
		}
};