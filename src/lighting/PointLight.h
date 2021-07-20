#include <shading/graph/ShaderGraph.h>
#include <shading/ScatterEvent.h>
#include <core/Scene.h>
#include "Light.h"

LAMBDA_BEGIN

class PointLight : public Light {
	public:
		Vec3 position;
		Real intensity;
		ShaderGraph::Socket *emission;

		PointLight();

		PointLight(const Vec3 &_position, ShaderGraph::Socket *_emission, const Real _intensity);

		Spectrum Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override;

		Real PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const override;

		Real PDF_Li(const ScatterEvent &_event) const override;

		Spectrum SamplePoint(Sampler &_sampler, ScatterEvent &_event, PartialLightSample *_ls) const override;

		Spectrum Visibility(const Vec3 &_shadingPoint, ScatterEvent &_event, Sampler &_sampler, PartialLightSample *_ls) const override;

		/*
			Abuse of terminology but convenient
		*/
		Real Irradiance() const override;

		/*
			Also abuse of terminology
		*/
		Real Area() const override;

		/*
			Infinitesimal box
		*/
		Bounds GetBounds() const override;

		/*
			No direction, however magnitude must be 1, so up is used - {0,1,0}
		*/
		Vec3 GetDirection() const override;
};

LAMBDA_END