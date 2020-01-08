#pragma once
#include <sampling/Piecewise.h>
#include <core/TriangleMesh.h>
#include <shading/graph/ShaderGraph.h>
#include <shading/SurfaceScatterEvent.h>
#include <core/Scene.h>
#include "Light.h"

class MeshLight : public Light {
	public:
		ShaderGraph::Socket *emission;
		Real intensity = 1;

		MeshLight(TriangleMesh *_mesh);

		Spectrum Sample_Li(SurfaceScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override;

		Real PDF_Li(const SurfaceScatterEvent &_event) const override;

		Spectrum L(const SurfaceScatterEvent &_event) const override;

		Real Area() const override;

		Real Irradiance() const override;

	protected:
		TriangleMesh *mesh;
		Distribution::Piecewise1D triDistribution;

		void InitDistribution();
};