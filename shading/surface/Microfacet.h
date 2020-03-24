#pragma once
#include "../graph/ShaderGraph.h"
#include "../Fresnel.h"
#include "../MicrofacetDistribution.h"

LAMBDA_BEGIN

class MicrofacetBRDF : public BxDF {
	public:
		Real etaT = 0.00001;
		ShaderGraph::Socket **albedoSocket, **roughnessSocket;
		MicrofacetDistribution *distribution;
		Fresnel *fresnel;


		MicrofacetBRDF(ShaderGraph::Socket **_albedoSocket, ShaderGraph::Socket **_roughnessSocket, MicrofacetDistribution *_distribution, Fresnel *_fresnel);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const override;

	private:
		inline Vec2 RoughnessToAlpha(const ScatterEvent *_event) const;
};

//class MicrofacetBSDF {};

LAMBDA_END