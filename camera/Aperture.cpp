#include "Aperture.h"

LAMBDA_BEGIN

BladeAperture::BladeAperture(const unsigned _blades, const Real _size) {
	blades = _blades;
	size = _size;
}

Vec2 BladeAperture::Sample_p(Real *_pdf) const {
	const Real d = PI2 / (Real)blades;
	const Real rnd = PI2 * sampler->Get1D();
	const Real thetaMin = std::floor(rnd / d) * d;
	const Real thetaMax = thetaMin + d;
	const Vec2 p1(std::cos(thetaMin), std::sin(thetaMin));
	const Vec2 p2(std::cos(thetaMax), std::sin(thetaMax));
	const Vec2 u = sampler->Get2D();
	return (p1 * u.x + p2 * u.y) * size;
}

CircularAperture::CircularAperture(const Real _size) {
	size = _size;
}

Vec2 CircularAperture::Sample_p(Real *_pdf) const {
	return Sampling::SampleUnitDisk(sampler->Get2D()) * size - Vec2(size * .5, size * .5);
}

MaskedAperture::MaskedAperture(Texture *_mask, Sampler *_sampler, const Real _size) {
	mask = _mask;
	sampler = _sampler;
	InitDistribution();
}

Vec2 MaskedAperture::Sample_p(Real *_pdf) const {
	return maskDistribution.SampleContinuous(sampler->Get2D(), _pdf) * size - Vec2(size * .5, size * .5);
}

void MaskedAperture::InitDistribution() {
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

LAMBDA_END