#pragma once
#include "BxDF.h"

LAMBDA_BEGIN

class OrenNayarBRDF : public BxDF {
	public:
		ShaderGraph::Socket **albedoSocket, **sigmaSocket;

		OrenNayarBRDF(ShaderGraph::Socket **_albedo, ShaderGraph::Socket **_sigma);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;
};



class OrenNayarBTDF : public BxDF {
	public:
		ShaderGraph::Socket **albedoSocket, **sigmaSocket;

		OrenNayarBTDF(ShaderGraph::Socket **_albedo, ShaderGraph::Socket **_sigma);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;
};

LAMBDA_END