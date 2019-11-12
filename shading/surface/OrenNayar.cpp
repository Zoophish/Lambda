#include "OrenNayar.h"

OrenNayarBRDF::OrenNayarBRDF(Texture *_albedo, const Real _sigma) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_DIFFUSE)) {
	albedo.SetTexture(_albedo);
	SetSigma(_sigma);
}

Spectrum OrenNayarBRDF::f(const SurfaceScatterEvent &_event) const {
	const Real thetaI = std::acos(maths::Clamp(_event.wiL.y, (Real)-1, (Real)1));
	const Real thetaO = std::acos(maths::Clamp(_event.woL.y, (Real)-1, (Real)1));
	const Real alpha = std::max(thetaI, thetaO);
	const Real beta = std::min(thetaI, thetaO);
	const Real phiI = std::acos(CosPhi(_event.wiL));
	const Real phiO = std::acos(CosPhi(_event.woL));
	return albedo.GetUV(_event.hit->uvCoords) * INV_PI * (A + B * std::max((Real)0, std::cos(phiI - phiO) * maths::Clamp(std::sin(alpha), (Real)-1, (Real)1) * maths::Clamp(std::tan(beta), (Real)-1, (Real)1)));
}

Spectrum OrenNayarBRDF::Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const {
	_event.wiL = Sampling::SampleCosineHemisphere(_u);
	if (_event.woL.y < 0) _event.wiL.y *= -1;
	_pdf = CosineHemispherePdf(_event.woL, _event.wiL);
	_event.pdf = _pdf;
	_event.wi = _event.ToWorld(_event.wiL);
	_event.hit->point += _event.hit->normalG * .00001;
	return f(_event);
}