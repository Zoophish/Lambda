#pragma once
#include "BxDF.h"

class GhostBTDF : public BxDF {
	public:
	ShaderGraph::Socket **alphaSocket;

	GhostBTDF(ShaderGraph::Socket **_alphaSocket);

	Spectrum f(const SurfaceScatterEvent &_event) const override;

	Spectrum Sample_f(SurfaceScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

	Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const override;
};