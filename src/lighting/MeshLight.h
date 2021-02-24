#pragma once
#include <sampling/Piecewise.h>
#include <core/TriangleMesh.h>
#include <shading/graph/ShaderGraph.h>
#include <shading/ScatterEvent.h>
#include <core/Scene.h>
#include "Light.h"

LAMBDA_BEGIN

class MeshLight : public Light {
	friend class TriangleLight;

	public:
		ShaderGraph::Socket *emission;
		Real intensity = 1;

		MeshLight();

		MeshLight(TriangleMesh *_mesh);

		Spectrum Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override;

		Real PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const override;

		Real PDF_Li(const ScatterEvent &_event) const override;

		Vec3 SamplePoint(Sampler &_sampler, ScatterEvent &_event, Real *_pdf) const override;

		Spectrum L(const ScatterEvent &_event) const override;

		Real Area() const override;

		Real Irradiance() const override;

		Bounds GetBounds() const override;

		Vec3 GetDirection() const override;

		TriangleMesh const &GetMesh() const;

	protected:
		TriangleMesh *mesh;
		Distribution::Piecewise1D triDistribution;

		void InitDistribution();
};

/*
	Required for primitive tree sampling of mesh lights.
*/
class TriangleLight : public Light {
	public:
		MeshLight *meshLight;
		size_t triIndex;

		TriangleLight(MeshLight *_meshLight, const size_t _i);

		Spectrum Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override;

		Real PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const override;

		Real PDF_Li(const ScatterEvent &_event) const override;

		Vec3 SamplePoint(Sampler &_sampler, ScatterEvent &_event, Real *_pdf) const override;

		Spectrum L(const ScatterEvent &_event) const override;

		Real Area() const override;

		Real Irradiance() const override;

		Bounds GetBounds() const override;

		Vec3 GetDirection() const override;
};

LAMBDA_END