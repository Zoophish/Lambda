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

		EnvironmentLight() {}

		EnvironmentLight(Texture *_texture, const Real _intesity = 1) {
			radianceMap.SetTexture(_texture);
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

		Spectrum Sample_Li(SurfaceScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override {
			const Vec2 uv = distribution->SampleContinuous(_sampler->Get2D(), &_pdf);
			if (_pdf == 0) return Spectrum(0);
			const Real theta = uv.y * PI + offset.y;
			const Real phi = uv.x * PI2 + offset.x;
			const Real cosTheta = std::cos(theta), sinTheta = std::sin(theta);
			if (sinTheta == 0) { _pdf = 0; }
			const Real sinPhi = std::sin(phi), cosPhi = std::cos(phi);
			_event.wi = maths::SphericalDirection(sinTheta, cosTheta, phi);
			if (_event.scene->RayEscapes(Ray(_event.hit->point + _event.hit->normalG * .00001, _event.wi))) {
				_event.wiL = _event.ToLocal(_event.wi);
				_pdf /= 2 * PI * PI * sinTheta;
				return radianceMap.GetUV(uv) * intensity;
			}
			_pdf = 0;
			return Spectrum(0);
		}

		Real PDF_Li(const SurfaceScatterEvent &_event) const override {
			const Real theta = maths::SphericalTheta(_event.wi) + offset.y;
			const Real phi = maths::SphericalPhi(_event.wi) + offset.x;
			const Real sinTheta = std::sin(theta);
			if (sinTheta == 0) return 0;
			const Vec3 wiOffset = maths::SphericalDirection(sinTheta, std::cos(theta), phi);
			if (_event.scene->RayEscapes(Ray(_event.hit->point + _event.hit->normalG * .00001, wiOffset))) {
				return distribution->PDF(maths::Fract(Vec2(phi * INV_PI2, theta * INV_PI))) / (2 * PI * PI * sinTheta);
			}
			return 0;
		}

		Spectrum Le(const Ray &_r) const override {
			return Le(_r.d);
		}

		//Aproximate disk.
		Real Area() const override {
			return PI * radius * radius;
		}

		Real Irradiance() const override {
			return intensity;
		}

		inline Spectrum Le(const Vec3 &_w) const {
			const Vec2 uv = maths::Fract(Vec2((maths::SphericalPhi(_w) - offset.x) * INV_PI2, (maths::SphericalTheta(_w) - offset.y) * INV_PI));
			return radianceMap.GetUV(uv) * intensity;
		}

	protected:
		TextureAdapter radianceMap;
		std::unique_ptr<Distribution::Piecewise2D> distribution;
};