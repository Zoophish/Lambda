#pragma once
#include "BxDF.h"

class OrenNayarBRDF : public BxDF {
	public:
		ShaderGraph::Socket **albedoSocket, **sigmaSocket;

		OrenNayarBRDF(ShaderGraph::Socket **_albedo, ShaderGraph::Socket **_sigma);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Sample_f(SurfaceScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;
};