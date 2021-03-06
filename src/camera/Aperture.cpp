#include "Aperture.h"

LAMBDA_BEGIN

BladeAperture::BladeAperture(const unsigned _blades, const Real _size) {
	blades = _blades;
	size = _size;
}

Vec2 BladeAperture::Sample_p(Sampler &_sampler, Real *_pdf) const {
	const Real d = PI2 / (Real)blades;
	const Real rnd = PI2 * _sampler.Get1D();
	const Real thetaMin = std::floor(rnd / d) * d;
	const Real thetaMax = thetaMin + d;
	const Vec2 p1(std::cos(thetaMin), std::sin(thetaMin));
	const Vec2 p2(std::cos(thetaMax), std::sin(thetaMax));
	const Vec2 u = _sampler.Get2D();
	return (p1 * u.x + p2 * u.y) * size;
}

CircularAperture::CircularAperture(const Real _size) {
	size = _size;
}

Vec2 CircularAperture::Sample_p(Sampler &_sampler, Real *_pdf) const {
	return Sampling::SampleUnitDisk(_sampler.Get2D()) * size - Vec2(size * .5, size * .5);
}

MaskedAperture::MaskedAperture(Texture *_mask, const Real _size) {
	InitDistribution(_mask);
}

Vec2 MaskedAperture::Sample_p(Sampler &_sampler, Real *_pdf) const {
	Real pdf;
	return maskDistribution.SampleContinuous(_sampler.Get2D(), &pdf) * size - Vec2(size * .5, size * .5);
}

void MaskedAperture::InitDistribution(Texture *_mask) {
	const unsigned w = _mask->GetWidth(), h = _mask->GetHeight();
	std::unique_ptr<Real[]> img(new Real[w * h]);
	for (unsigned y = 0; y < h; ++y) {
		const Real vp = (Real)y / (Real)h;
		for (unsigned x = 0; x < w; ++x) {
			Real up = (Real)x / (Real)w;
			img[x + y * w] = std::abs(_mask->GetPixelUV(up, vp).r);
		}
	}
	maskDistribution = Distribution::Piecewise2D(img.get(), w, h);
}

LAMBDA_END