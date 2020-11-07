#pragma once
#include "../graph/ShaderGraph.h"
#include "../ScatterEvent.h"
#include <sampling/Sampling.h>
#include <sampling/Sampler.h>

LAMBDA_BEGIN

class BxDF {
	public:
		/*
			BxDF flags which describe the bxdf's overall characteristic
		*/
		enum BxDFType {
			BxDF_REFLECTION = BITFLAG(0),
			BxDF_TRANSMISSION = BITFLAG(1),
			BxDF_DIFFUSE = BITFLAG(2),
			BxDF_GLOSSY = BITFLAG(3),
			BxDF_SPECULAR = BITFLAG(4),
		};

		const BxDFType type;

		BxDF(const BxDFType _type);

		/*
			The bxdf function of _event->woL and _event->wiL, with access to other shading parameters in _event
		*/
		virtual Spectrum f(const ScatterEvent &_event) const = 0;

		/*
			Samples a scattering direction, storing it in _event->wiL and _event->wi and returns the respective bxdf
			for this direction.
			The default method will sample the cosine weighted hemisphere and always flip the direction to the side
			of the face the shading event has occurred on.
		*/
		virtual Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const;

		/*
			Computes the hemispherical-directional reflectance or hemispherical-hemispherical reflectance function
		*/
		virtual Spectrum Rho(const ScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const;

		/*
			Returns the probability of choosing directions _wo and _wi in Sample_f
		*/
		virtual Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const;	//First two arguments redundant but need checking.

		static inline Real CosineHemispherePdf(const Vec3 &_wo, const Vec3 &_wi) {
			return SameHemisphere(_wo, _wi) ? std::abs(_wi.y) * INV_PI : 0;
		}
};



class LambertianBRDF : public BxDF {
	public:
		ShaderGraph::Socket **albedoSocket;

		LambertianBRDF(ShaderGraph::Socket **_albedoSocket);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Rho(const ScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const override;
};



class MixBSDF : public BxDF {
	public:
		ShaderGraph::Socket **aSocket, **bSocket, **ratioSocket;

		MixBSDF(ShaderGraph::Socket **_aSocket, ShaderGraph::Socket **_bSocket, ShaderGraph::Socket **_ratioSocket);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const override;
};

LAMBDA_END