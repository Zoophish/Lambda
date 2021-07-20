#pragma once
#include <math.h>
#include "HomogeneousMedium.h"

LAMBDA_BEGIN

HomogeneousMedium::HomogeneousMedium(const Spectrum &_sigmaA, const Spectrum &_sigmaS) {
	sigmaA = _sigmaA;
	sigmaS = _sigmaS;
}

Spectrum HomogeneousMedium::Tr(const Ray &_ray, const Real _tFar, Sampler &_sampler) const {
	const Spectrum sigmaT = sigmaA + sigmaS;
	return Spectrum::Exp(-sigmaT * std::min(_tFar, MAX_REAL));
}

Spectrum HomogeneousMedium::SampleDistance(const Ray &_ray, Sampler &_sampler, ScatterEvent &_event, Real *_t, Real *_pdf) const {
	const Spectrum sigmaT = sigmaA + sigmaS;
	const Vec2 u = _sampler.Get2D();
	const unsigned channel = std::min((unsigned)(u.x * Spectrum::nSamples), Spectrum::nSamples - 1);
	const Real dist = -std::log(1 - u.y) / sigmaT[channel];
	const Real t = std::min(dist, _event.hit->tFar);
	_event.mediumInteraction = t < _event.hit->tFar;

	//if (_event.mediumInteraction) _event.hit->point = _ray.o + _ray.d * t;

	const Spectrum Tr = Spectrum::Exp(-sigmaT * std::min(t, MAX_REAL));

	// Return weighting factor for scattering from homogeneous medium
	const Spectrum density = _event.mediumInteraction ? (sigmaT * Tr) : (Spectrum)Tr;
	Real pdf = 0;
	for (int i = 0; i < Spectrum::nSamples; ++i) pdf += density[i];	//Take average of p values for each spectrum channel
	pdf *= 1 / (Real)Spectrum::nSamples;	//Division determined at compile time
	if (pdf == 0) pdf = 1;
	if (_t) *_t = t;
	if (_pdf) *_pdf = pdf;
	return _event.mediumInteraction ? (Tr * sigmaS / pdf) : (Tr / pdf);
}

Real HomogeneousMedium::PDFDistance(const Real _t) const {
	const Spectrum sigmaT = sigmaA + sigmaS;
	const Spectrum Tr = Spectrum::Exp(-sigmaT * std::min(_t, MAX_REAL));
	Real pdf = 0;
	for (int i = 0; i < Spectrum::nSamples; ++i) pdf += sigmaT[i] * Tr[i];
	pdf *= 1 / Spectrum::nSamples;
	return pdf;
}

Spectrum HomogeneousMedium::SampleEquiangular(const Ray &_ray, Sampler &_sampler, ScatterEvent &_event, const Vec3 &_lightPoint, Real *_t, Real *_pdf) const {
	const Vec3 delta = _lightPoint - _event.hit->point;
	const Real l = maths::Dot(_ray.d, delta);	//Distance along _ray to perpendicular bisection from _lightPoint
	const Real D = std::sqrt(maths::Dot(delta, delta) - l * l);
	//if (D == 0) {
		//https://fpsunflower.github.io/ckulla/data/volume-rendering-techniques.pdf
	//}

	/*
		TODO: Account for cone of influence of lights (mesh lights and spotlights)
	*/
	const Real thetaA = -std::atan2(l, D);	//Angles representing light's subtention of ray segment (might be smaller for spotlights).
	const Real thetaB = std::atan2(_event.hit->tFar - l, D);

	const Real epsilon = _sampler.Get1D();
	const Real dist = D * std::tan(maths::Lerp(thetaB, thetaA, epsilon));
	const Real t = std::min(t, _event.hit->tFar);
	*_t = t;

	_event.mediumInteraction = t < _event.hit->tFar;

	const Vec2 u = _sampler.Get2D();
	const unsigned channel = std::min((unsigned)(u.x * Spectrum::nSamples), Spectrum::nSamples - 1);

	const Real pdf = D / ((thetaB - thetaA) * (D * D + t * t));
	*_pdf = pdf;
	const Spectrum sigmaT = sigmaA + sigmaS;
	const Spectrum Tr = Spectrum::Exp(-sigmaT * std::min(t, MAX_REAL));
	return Tr * sigmaS / pdf;
}

Real HomogeneousMedium::PDFEquiangular(const Ray &_ray, const Vec3 &_lightPoint, const Real _tFar, const Real _t) const {
	const Vec3 delta = _lightPoint - _ray.o;
	const Real l = maths::Dot(_ray.d, delta);
	const Real D = std::sqrt(maths::Dot(delta, delta) - l * l);
	/*
		TODO: Account for light cone of influence
	*/
	const Real thetaA = -std::atan2(l, D);
	const Real thetaB = std::atan2(_tFar - l, D);

	const Real pdf = D / ((thetaB - thetaA) * (D * D + _t * _t));
	return pdf;
}

LAMBDA_END