#pragma once
#include <image/Texture.h>
#include <sampling/Piecewise.h>
#include <sampling/Sampler.h>
#include <maths/maths.h>

class Aperture {
	public:
		Sampler *sampler;
		Real size;

		virtual Vec2 Sample_p(Real *_pdf = nullptr) const = 0;
};

class BladeAperture : public Aperture {
	public:
	unsigned blades;

	BladeAperture(const unsigned _blades, const Real _size = 1) {
		blades = _blades;
		size = _size;
	}

	Vec2 Sample_p(Real *_pdf = nullptr) const override {
		const Real rnd = PI2 * sampler->Get1D() / (Real)blades;
		const Real thetaMin = std::floor(rnd) * (Real)blades;
		const Real thetaMax = std::ceil(rnd) * (Real)blades;
		const Vec2 p1(std::cos(thetaMin), std::sin(thetaMin));
		const Vec2 p2(std::cos(thetaMax), std::sin(thetaMax));
		const Vec2 u = sampler->Get2D();
		return (p1 * u.x + p2 * u.y) * size;
	}
};

class MaskedAperture : public Aperture {
	public:

		MaskedAperture(Texture *_mask, Sampler *_sampler, const Real _size = 1) {
			mask = _mask;
			sampler = _sampler;
			InitDistribution();
		}

		Vec2 Sample_p(Real *_pdf = nullptr) const override {
			return maskDistribution.SampleContinuous(sampler->Get2D(), _pdf) * size - Vec2(size * .5, size *.5);
		}

	protected:
		Texture *mask;
		Distribution::Piecewise2D maskDistribution;
		
		void InitDistribution() {
			const unsigned w = mask->GetWidth(), h = mask->GetHeight();
			std::unique_ptr<Real[]> img(new Real[w * h]);
			for (unsigned y = 0; y < h; ++y) {
				const Real vp = (Real)y / (Real)h;
				for (unsigned x = 0; x < w; ++x) {
					Real up = (Real)x / (Real)w;
					img[x + y * w] = std::abs(mask->GetPixelUV(up, vp).r);
				}
			}
			maskDistribution = Distribution::Piecewise2D(img.get(), w, h);
		}
};