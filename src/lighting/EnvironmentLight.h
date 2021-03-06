#pragma once
#include "Light.h"
#include <shading/TextureAdapter.h>
#include <sampling/Piecewise.h>
#include <shading/ScatterEvent.h>
#include <core/Ray.h>
#include <core/Scene.h>

LAMBDA_BEGIN

class EnvironmentLight : public Light {
	public:
		Vec2 offset;
		Real intensity = 1;
		Real radius;
		Bounds bounds;

		EnvironmentLight();

		EnvironmentLight(Texture *_texture, const Real _intesity = 1);

		Spectrum Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override;

		Real PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const override;

		Real PDF_Li(const ScatterEvent &_event) const override;

		Spectrum SamplePoint(Sampler &_sampler, ScatterEvent &_event, PartialLightSample *_ls) const override;

		Spectrum Visibility(const Vec3 &_shadingPoint, ScatterEvent &_event, Sampler &_sampler, PartialLightSample *_ls) const override;

		Spectrum Le(const Ray &_r) const override;

		Spectrum L(const ScatterEvent &_event) const override;

		Real Area() const override;

		Real Irradiance() const override;

		Bounds GetBounds() const override;

		Vec3 GetDirection() const override;

		inline Spectrum Le(const Vec3 &_w) const {
			const Vec2 uv = maths::Fract(Vec2((maths::SphericalPhi(_w) - offset.x) * INV_PI2, (maths::SphericalTheta(_w) - offset.y) * INV_PI));
			return radianceMap.GetUV(uv) * intensity;
		}

	protected:
		TextureAdapter radianceMap;
		std::unique_ptr<Distribution::Piecewise2D> distribution;
};

LAMBDA_END