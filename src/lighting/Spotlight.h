#include <shading/graph/ShaderGraph.h>
#include <shading/ScatterEvent.h>
#include <core/Scene.h>
#include "Light.h"

LAMBDA_BEGIN

class Spotlight : public Light {
	public:
		Real intensity, cosConeAngle, cosFalloffStart;
		Vec3 axis;
		ShaderGraph::Socket *emission;

		Spotlight();

		Spotlight(const Vec3 &_axis, const Real _coneAngle, const Real _falloffStart);

		Spectrum Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override;

		Real PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const override;

		Real PDF_Li(const ScatterEvent &_event) const override;

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

		Vec3 GetDirection() const override;
};

LAMBDA_END