#pragma once
#include "Fresnel.h"
#include "BxDF.h"

class FresnelBSDF : public BxDF {
	public:
		Real ior;
		TextureAdapter albedo;

		FresnelBSDF(Texture *_albedo, const Real _ior) : BxDF((BxDFType)(BxDF_TRANSMISSION | BxDF_SPECULAR)) {
			ior = _ior;
			albedo.SetTexture(_albedo);
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return Spectrum(0);
		}

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override {
			const bool entering = _event.woL.y > 0;
			const Real a = entering ? _event.eta : ior;
			const Real b = entering ?ior : _event.eta;
			const Real fr = Fresnel::FrDielectric(_event.woL.y, a, b);
			if (_u.x < fr) {
				_event.wiL = Vec3(-_event.woL.x, _event.woL.y, -_event.woL.z);
				_event.wi = _event.ToWorld(_event.wiL);
				const Real cosTheta = std::abs(_event.wiL.y);
				_event.pdf = fr;
				return albedo.GetUV(_event.hit->uvCoords) * fr / cosTheta;
			}
			else {
				const bool entering = _event.woL.y > 0;
				const Real etaI = entering ? _event.eta : ior;
				const Real etaT = entering ? ior : _event.eta;
				_event.pdf = 1 - fr;
				_event.wiL = Refract(_event.woL, Vec3(0, 1, 0) * (entering ? 1 : -1), etaI / etaT);
				_event.wi = _event.ToWorld(_event.wiL);
				const Real cosTheta = std::abs(_event.wiL.y);
				Spectrum ft = albedo.GetUV(_event.hit->uvCoords) * (1 - fr);
				ft *= (etaI * etaI) / (etaT * etaT);
				_event.hit->point += _event.wi * .001;
				return ft / cosTheta;
			}
		}

	protected:
		inline Vec3 Refract(const Vec3 &_w, const Vec3 &_n, const Real _eta) const {
			const Real cosThetaI = maths::Dot(_w, _n);
			const Real sin2ThetaI = std::max((Real)0, (Real)1 - cosThetaI * cosThetaI);
			const Real sin2ThetaT = _eta * _eta * sin2ThetaI;
			if (sin2ThetaT >= 1) { //TIR

			}
			const Real cosThetaT = std::sqrt(1 - sin2ThetaT);
			return -_w * _eta + _n * (_eta * cosThetaI - cosThetaT);
		}
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
			_event.wiL = Vec3(-_event.woL.x, _event.woL.y, -_event.woL.z);
			const Real cosTheta = _event.wiL.y;
			_event.wi = _event.ToWorld(_event.wiL);
			return fresnel->Evaluate(cosTheta, _event.eta) * albedo.GetUV(_event.hit->uvCoords) / std::abs(cosTheta);
		}
};

class SpecularBTDF : public BxDF {
	public:
		FresnelDielectric fresnel;
		TextureAdapter albedo;

		SpecularBTDF(Texture *_albedo, const Real _etaT) : BxDF((BxDFType)(BxDF_TRANSMISSION | BxDF_SPECULAR)) {
			albedo.SetTexture(_albedo);
			fresnel.etaT = _etaT;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return Spectrum(0);
		}

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override {
			_event.pdf = 1;
			const Vec3 woL = _event.ToLocal(_event.wo);
			const bool outside = woL.y > 0;
			_event.hit->normalS *= (outside ? 1 : -1);
			const Real etaI = outside ? _event.eta : fresnel.etaT;
			const Real etaT = outside ? fresnel.etaT : _event.eta;
			_event.wi = Refract(_event.wo, Vec3(0,1,0) * (outside ? 1 : -1), etaI / etaT).Normalised();
			Spectrum ft = albedo.GetUV(_event.hit->uvCoords) * (Spectrum(1) - fresnel.Evaluate(std::abs(_event.wi.y), etaT));
			ft *= (etaI * etaI) / (etaT * etaT);
			const Real inv = (Real)1 / std::abs(_event.wi.y);
			_event.wi = _event.ToWorld(_event.wi);
			_event.hit->point += _event.hit->normalS * .0004;
			return ft * inv;
		}

	protected:
		inline Vec3 Reflect(const Vec3 &_w, const Vec3 &_n) const {
			return (_n * -2 * maths::Dot(_w, _n)) + _w;
		}

		inline Vec3 Refract(const Vec3 &_w, const Vec3 &_n, const Real _eta) const {
			const Real cosThetaI = maths::Dot(_w, _n);
			const Real sin2ThetaI = std::max((Real)0, (Real)1 - cosThetaI * cosThetaI);
			const Real sin2ThetaT = _eta * _eta * sin2ThetaI;
			if (sin2ThetaT >= 1) { //TIR
				return Reflect(_w, _n);
			}
			const Real cosThetaT = std::sqrt(1 - sin2ThetaT);
			return -_w * _eta + _n * (_eta * cosThetaI - cosThetaT);
		}
};