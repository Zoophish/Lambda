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

Spectrum EnvironmentLight::Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const {
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
	const Real ep = SURFACE_EPSILON;// maths::Dot(_event.wi, _event.hit->normalG) < 0 ? -.00001 : .00001;// _event.woL.y < 0 ? -.00001 : .00001;	Can we assume bsdf will zero out wrong side?
	Spectrum Tr(1);
	if (Light::RayEscapes(Ray(_event.hit->point + _event.hit->normalG * ep, _event.wi), _event, *_sampler, &Tr)) {
		_event.wiL = _event.ToLocal(_event.wi);
		_pdf /= 2 * PI * PI * sinTheta;
		return radianceMap.GetUV(uv) * Tr * intensity;
	}
	_pdf = 0;
	return Spectrum(0);
}

Real EnvironmentLight::PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const {
	const Real theta = maths::SphericalTheta(_event.wi) - offset.y;
	const Real phi = maths::SphericalPhi(_event.wi) - offset.x;
	const Real sinTheta = std::sin(theta);
	if (sinTheta == 0) return 0;
	const Vec3 wiOffset = maths::SphericalDirection(sinTheta, std::cos(theta), phi);
	//const Real ep = _event.wiL.y < 0 ? -.00001 : .00001;	//We assume that the bxdf has handled pushing the hit.point to the correct side
	if (RayEscapes(Ray(_event.hit->point, wiOffset), _event, _sampler)) {
		return distribution->PDF(maths::Fract(Vec2(phi * INV_PI2, theta * INV_PI))) / ((Real)2 * PI * PI * sinTheta);
	}
	return 0;
}

Real EnvironmentLight::PDF_Li(const ScatterEvent &_event) const {
	const Real theta = maths::SphericalTheta(_event.wi) - offset.y;
	const Real phi = maths::SphericalPhi(_event.wi) - offset.x;
	const Real sinTheta = std::sin(theta);
	if (sinTheta == 0) return 0;
	const Vec3 wiOffset = maths::SphericalDirection(sinTheta, std::cos(theta), phi);
	return distribution->PDF(maths::Fract(Vec2(phi * INV_PI2, theta * INV_PI))) / ((Real)2 * PI * PI * sinTheta);
}

Spectrum EnvironmentLight::SamplePoint(Sampler &_sampler, ScatterEvent &_event, PartialLightSample *_ls) const {
	//TODO
	_ls->pdf *= 0;
	_ls->point = Vec3(0, 0, 0);	// unit vector in direction of ray
	return Spectrum(0);
}

Spectrum EnvironmentLight::Visibility(const Vec3 &_shadingPoint, ScatterEvent &_event, Sampler &_sampler, PartialLightSample *_ls) const {
	const Vec3 wi = (_ls->point - _shadingPoint).Normalised();
	Spectrum Tr(1);
	if (RayEscapes(Ray(_shadingPoint, wi), _event, _sampler, &Tr)) {
		return Tr;
	}
	_ls->pdf = 0;
	return Spectrum(0);
}

Spectrum EnvironmentLight::Le(const Ray &_r) const {
	return Le(_r.d);
}

Spectrum EnvironmentLight::L(const ScatterEvent &_event) const {
	return Le(_event.wi);
}

Real EnvironmentLight::Area() const {
	return PI * radius * radius;
}

Real EnvironmentLight::Irradiance() const {
	return intensity;
}

Bounds EnvironmentLight::GetBounds() const {
	return bounds;
}

Vec3 EnvironmentLight::GetDirection() const {
	return Vec3(0, 0, 0);
}

LAMBDA_END