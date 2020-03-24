#pragma once
#include <sampling/Piecewise.h>
#include <core/TriangleMesh.h>
#include <shading/graph/ShaderGraph.h>
#include <shading/ScatterEvent.h>
#include <core/Scene.h>
#include "Light.h"

LAMBDA_BEGIN

class MeshLight : public Light {
	public:
		ShaderGraph::Socket *emission;
		Real intensity = 1;

		MeshLight(TriangleMesh *_mesh);

		Spectrum Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override;

		Real PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const override;

		Spectrum L(const ScatterEvent &_event) const override;

		Real Area() const override;

		Real Irradiance() const override;

	protected:
		TriangleMesh *mesh;
		Distribution::Piecewise1D triDistribution;

		void InitDistribution();
};

LAMBDA_END