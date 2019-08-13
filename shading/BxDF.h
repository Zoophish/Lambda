#pragma once
#include <image/Texture.h>
#include "Fresnel.h"
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

		virtual Spectrum Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2* _smpls) const;

		//----	Utility functions:	----
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
		inline Spectrum RGBToSpec(const Real _r, const Real _g, const Real _b, const SpectrumType _type) const {
			const Real rgb[3] = { _r, _g, _b };
			return Spectrum::FromRGB(rgb, _type);
		}
};

//Ad hoc interface to deal with RGB textures in a Spectral renderer.
class TextureAdapter {
	public:
		Texture *texture;
		SpectrumType type;

		TextureAdapter() {
			texture = nullptr;
			type = SpectrumType::Reflectance;
		}

		TextureAdapter(Texture *_texture, const SpectrumType _type = SpectrumType::Reflectance) {
			texture = _texture;
			type = _type;
		}

		Spectrum GetUV(const Vec2 &_uv) const {
			const Colour c = texture->GetPixelUV(_uv.x, _uv.y);
			const float tmp[3] = { c.r, c.g, c.b };
			return Spectrum::FromRGB(tmp, type);
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