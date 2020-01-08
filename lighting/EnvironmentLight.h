#pragma once
#include "Light.h"
#include <shading/TextureAdapter.h>
#include <sampling/Piecewise.h>
#include <shading/SurfaceScatterEvent.h>
#include <core/Ray.h>
#include <core/Scene.h>

class EnvironmentLight : public Light {
	public:
		Vec2 offset;
		Real intensity = 1;
		//Real radius;

		EnvironmentLight();

		EnvironmentLight(Texture *_texture, const Real _intesity = 1);

		Spectrum Sample_Li(SurfaceScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override;

		Real PDF_Li(const SurfaceScatterEvent &_event) const override;

		Spectrum Le(const Ray &_r) const override;

		Real Area() const override;

		Real Irradiance() const override;

		inline Spectrum Le(const Vec3 &_w) const {
			const Vec2 uv = maths::Fract(Vec2((maths::SphericalPhi(_w) + offset.x) * INV_PI2, (maths::SphericalTheta(_w) + offset.y) * INV_PI));
			return radianceMap.GetUV(uv) * intensity;
		}

	protected:
		TextureAdapter radianceMap;
		std::unique_ptr<Distribution::Piecewise2D> distribution;
};