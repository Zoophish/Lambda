#include "EnvironmentLight.h"

LAMBDA_BEGIN

EnvironmentLight::EnvironmentLight() {}

EnvironmentLight::EnvironmentLight(Texture *_texture, const Real _intesity) {
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

static inline bool RayEscapes(const Ray &_r, const SurfaceScatterEvent &_event, Sampler &_sampler, Spectrum *_Tr = nullptr) {
	if (_event.scene->hasVolumes) {
		RayHit hit;
		return !_event.scene->IntersectTr(_r, hit, _sampler, _event.medium, _Tr);
	}
	return _event.scene->RayEscapes(_r);
}

Spectrum EnvironmentLight::Sample_Li(SurfaceScatterEvent &_event, Sampler *_sampler, Real &_pdf) const {
	const Vec2 uv = distribution->SampleContinuous(_sampler->Get2D(), &_pdf);
	if (_pdf == 0) return Spectrum(0);
	const Real theta = uv.y * PI + offset.y;
	const Real phi = uv.x * PI2 + offset.x;
	const Real cosTheta = std::cos(theta), sinTheta = std::sin(theta);
	if (sinTheta == 0) { 
		_pdf = 0;
		return Spectrum(0);
	}
	const Real sinPhi = std::sin(phi), cosPhi = std::cos(phi);
	_event.wi = maths::SphericalDirection(sinTheta, cosTheta, phi);
	const Real ep = _event.woL.y < 0 ? -.00001 : .00001;
	Spectrum Tr(1);
	if (RayEscapes(Ray(_event.hit->point + _event.hit->normalG * ep, _event.wi), _event, *_sampler, &Tr)) {
		_event.wiL = _event.ToLocal(_event.wi);
		_pdf /= 2 * PI * PI * sinTheta;
		return radianceMap.GetUV(uv) * Tr * intensity;
	}
	_pdf = 0;
	return Spectrum(0);
}

Real EnvironmentLight::PDF_Li(const SurfaceScatterEvent &_event, Sampler &_sampler) const {
	const Real theta = maths::SphericalTheta(_event.wi) - offset.y;
	const Real phi = maths::SphericalPhi(_event.wi) - offset.x;
	const Real sinTheta = std::sin(theta);
	if (sinTheta == 0) return 0;
	const Vec3 wiOffset = maths::SphericalDirection(sinTheta, std::cos(theta), phi);
	const Real ep = _event.woL.y < 0 ? -.00001 : .00001;
	if (RayEscapes(Ray(_event.hit->point + _event.hit->normalG * ep, wiOffset), _event, _sampler)) {
		return distribution->PDF(maths::Fract(Vec2(phi * INV_PI2, theta * INV_PI))) / ((Real)2 * PI * PI * sinTheta);
	}
	return 0;
}

Spectrum EnvironmentLight::Le(const Ray &_r) const {
	return Le(_r.d);
}

Real EnvironmentLight::Area() const {
	return PI * radius * radius;
}

Real EnvironmentLight::Irradiance() const {
	return intensity;
}

LAMBDA_END