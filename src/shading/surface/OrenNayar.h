#pragma once
#include "BxDF.h"

LAMBDA_BEGIN

class OrenNayarBRDF : public BxDF {
	public:
		ShaderGraph::SocketRef *albedoSocket, *sigmaSocket;

		OrenNayarBRDF(ShaderGraph::SocketRef *_albedo, ShaderGraph::SocketRef *_sigma);

		Spectrum f(const ScatterEvent &_event) const override;

		//Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;
};



class OrenNayarBTDF : public BxDF {
	public:
		ShaderGraph::SocketRef *albedoSocket, *sigmaSocket;

		OrenNayarBTDF(ShaderGraph::SocketRef *_albedo, ShaderGraph::SocketRef *_sigma);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;
};

LAMBDA_END