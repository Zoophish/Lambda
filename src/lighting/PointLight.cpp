#include "PointLight.h"

LAMBDA_BEGIN

PointLight::PointLight() {
	position = Vec3(0, 0, 0);
	emission = nullptr;
	intensity = 1;
}

PointLight::PointLight(const Vec3 &_position, ShaderGraph::Socket *_emission, const Real _intensity) {
	position = _position;
	emission = _emission;
	intensity = _intensity;
}


Spectrum PointLight::Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const {
	const Vec3 p = _event.hit->point + _event.hit->normalG * SURFACE_EPSILON * _event.sidedness;
	Spectrum Tr(1);
	if (PointMutualVisibility(p, position, _event, *_event.scene, *_sampler, &Tr)) {
		_pdf = 1;
		const Real dist2 = maths::DistSq(position, _event.hit->point);
		_event.wiL = _event.ToLocal(_event.wi);	//Localise the sampled direction, wi given by MutualVisibility()
		return emission->GetAsSpectrum(_event) * Tr * intensity / dist2;
	}
	_pdf = 0;
	return Spectrum(0);
}


Real PointLight::PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const {
	return 0;
}

Real PointLight::PDF_Li(const ScatterEvent &_event) const {
	return 0;
}

Real PointLight::Irradiance() const {
	return intensity;
}

Real PointLight::Area() const {
	return 4 * PI;
}

Bounds PointLight::GetBounds() const {
	return Bounds(position);
}

Vec3 PointLight::GetDirection() const {
	return Vec3(0, 1, 0);
}

LAMBDA_END