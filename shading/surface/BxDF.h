#pragma once
#include "../graph/ShaderGraph.h"
#include "../SurfaceScatterEvent.h"
#include <sampling/Sampling.h>
#include <sampling/Sampler.h>

#define SURFACE_EPSILON 1e-5

LAMBDA_BEGIN

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

		virtual Spectrum Sample_f(SurfaceScatterEvent &_event, Sampler &_sampler, Real &_pdf) const;

		virtual Spectrum Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const;

		virtual Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const;	//First two arguments redundant but need checking.

		static inline Real CosineHemispherePdf(const Vec3 &_wo, const Vec3 &_wi) {
			return SameHemisphere(_wo, _wi) ? std::abs(_wi.y) * INV_PI : 0;
		}
};



class LambertianBRDF : public BxDF {
	public:
		ShaderGraph::Socket **albedoSocket;

		LambertianBRDF(ShaderGraph::Socket **_albedoSocket);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const override;
};



class MixBSDF : public BxDF {
	public:
		ShaderGraph::Socket **aSocket, **bSocket, **ratioSocket;

		MixBSDF(ShaderGraph::Socket **_aSocket, ShaderGraph::Socket **_bSocket, ShaderGraph::Socket **_ratioSocket);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Sample_f(SurfaceScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const override;
};

LAMBDA_END