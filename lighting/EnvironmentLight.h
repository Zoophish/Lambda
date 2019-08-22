#pragma once
#include <shading/TextureAdapter.h>
#include <sampling/Piecewise.h>
#include <shading/SurfaceScatterEvent.h>
#include <core/Ray.h>
#include "Light.h"

class EnvironmentLight : public Light {
	public:
		Vec2 offset;
		Real intensity = 1;

		EnvironmentLight() {}

		EnvironmentLight(Texture *_texture, const Real _intesity = 1) {
			radianceMap.texture = _texture;
			radianceMap.type = SpectrumType::Illuminant;
			const unsigned w = _texture->GetWidth(), h = _texture->GetHeight();
			std::unique_ptr<Real[]> img(new Real[w * h]);
			for (unsigned y = 0; y < h; ++y) {
				const Real vp = (Real)y / (Real)h;
				const Real sinTheta = std::sin(PI * Real(y + .5) / Real(h));
				for (unsigned x = 0; x < w; ++x) {
					Real up = (Real)x / (Real)w;
					img[x + y * w] = std::abs(radianceMap.GetUV(Vec2(up, vp)).y());
					img[x + y * w] *= sinTheta;
				}
			}
			distribution.reset(new Distribution::Piecewise2D(img.get(), w, h));
		}

		Spectrum Sample_Li(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override {
			const Vec2 uv = distribution->SampleContinuous(_u, &_pdf);
			if (_pdf == 0) return Spectrum(0);
			const Real theta = uv.y * PI + offset.y, phi = uv.x * PI2 + offset.x;
			const Real cosTheta = std::cos(theta), sinTheta = std::sin(theta);
			const Real sinPhi = std::sin(phi), cosPhi = std::cos(phi);
			_event.wi = maths::SphericalDirection(sinTheta, cosTheta, phi).Normalised();
			if (sinTheta == 0) { _pdf = 0; }
			else { _pdf /= 2 * PI * PI * sinTheta; }
			return radianceMap.GetUV(uv) * intensity;
		}

		Real PDF_Li(const SurfaceScatterEvent &_event) const override {
			const Real theta = maths::SphericalTheta(_event.wi) + offset.y, phi = maths::SphericalPhi(_event.wi) + offset.x;
			const Real sinTheta = std::sin(theta);
			if (sinTheta == 0) return 0;
			return distribution->PDF(Vec2(phi * INV_PI2, theta * INV_PI).Mod()) / (2 * PI * PI * sinTheta);
		}

		Spectrum Le(const Ray &_r) const override {
			const Vec2 uv = Vec2((maths::SphericalPhi(_r.d) + offset.x) * INV_PI2, (maths::SphericalTheta(_r.d) + offset.y) * INV_PI);
			return radianceMap.GetUV(uv) * intensity;
		}

	protected:
		TextureAdapter radianceMap;
		std::unique_ptr<Distribution::Piecewise2D> distribution;
};