#include <shading/graph/ShaderGraph.h>
#include <shading/ScatterEvent.h>
#include <core/Scene.h>
#include "Light.h"

LAMBDA_BEGIN

class Spotlight : public Light {
	public:
		Real intensity, cosConeAngle, cosFalloffStart;
		Vec3 axis, position;
		ShaderGraph::Socket *emission;

		Spotlight();

		Spotlight(const Vec3 &_position, const Vec3 &_axis, const Real _coneAngle, const Real _falloffStart);

		/*
			Returns intensity factor of point light from direction _w.
		*/
		Real Falloff(const Vec3 &_w) const;

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

		Vec3 GetDirection() const override;
};

LAMBDA_END