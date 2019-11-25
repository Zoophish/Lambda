#pragma once
#include "BxDF.h"

class GhostBTDF : public BxDF {
	public:
		ShaderGraph::Socket **alphaSocket;

		GhostBTDF(ShaderGraph::Socket **_alphaSocket);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override;
};