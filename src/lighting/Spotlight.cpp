#include "Spotlight.h"

LAMBDA_BEGIN

Spotlight::Spotlight() {
	position = Vec3(0, 0, 0);
	axis = Vec3(0, -1, 0);
	cosConeAngle = std::cos(PI * .5);
	cosFalloffStart = std::cos(PI * .4);
	intensity = 1;
	emission = nullptr;
}

Spotlight::Spotlight(const Vec3 &_position, const Vec3 &_axis, const Real _coneAngle, const Real _falloffStart) : Spotlight() {
	position = _position;
	axis = _axis;
	cosConeAngle = std::cos(_coneAngle);
	cosFalloffStart = std::cos(_falloffStart);
}

Real Spotlight::Falloff(const Vec3 &_w) const {
	const Real cosTheta = maths::Dot(_w, axis);
	if (cosTheta < cosConeAngle) return 0;
	if (cosTheta > cosFalloffStart) return 1;
	const Real d = (cosTheta - cosConeAngle) / (cosFalloffStart - cosConeAngle);
	return (d * d) * (d * d);
}

Spectrum Spotlight::Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const {
	const Vec3 p = _event.hit->point + _event.hit->normalG * SURFACE_EPSILON * _event.sidedness;
	Spectrum Tr(1);
	if (PointMutualVisibility(p, position, _event, *_event.scene, *_sampler, &Tr)) {
		_pdf = 1;
		const Real dist2 = maths::DistSq(position, _event.hit->point);
		_event.wiL = _event.ToLocal(_event.wi);	//Localise the sampled direction, wi, given by PointMutualVisibility()
		return emission->GetAsSpectrum(_event) * Tr * Falloff(-_event.wi) * intensity / dist2;
	}
	_pdf = 0;
	return Spectrum(0);
}

Real Spotlight::PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const {
	return 0;
}

Real Spotlight::PDF_Li(const ScatterEvent &_event) const {
	return 0;
}

Real Spotlight::Irradiance() const {
	return intensity;
}

Real Spotlight::Area() const {
	return PI2 - (1 - (Real).5 * (cosFalloffStart + cosConeAngle));
}

Bounds Spotlight::GetBounds() const {
	return Bounds(position);
}

Vec3 Spotlight::GetDirection() const {
	return axis;
}

LAMBDA_END