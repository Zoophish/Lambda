#pragma once
#include "BxDF.h"

LAMBDA_BEGIN

class GhostBTDF : public BxDF {
	public:
	ShaderGraph::Socket **alphaSocket;

	GhostBTDF(ShaderGraph::Socket **_alphaSocket);

	Spectrum f(const ScatterEvent &_event) const override;

	Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

	Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const override;
};

LAMBDA_END