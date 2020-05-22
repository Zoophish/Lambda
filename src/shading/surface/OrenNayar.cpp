#include "OrenNayar.h"

LAMBDA_BEGIN

OrenNayarBRDF::OrenNayarBRDF(ShaderGraph::Socket **_albedo, ShaderGraph::Socket **_sigma) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_DIFFUSE)) {
	albedoSocket = _albedo;
	sigmaSocket = _sigma;
}

static inline Real OrenNayarTerm(const ScatterEvent &_event, const Real _sigma) {
	const Real thetaI = std::acos(maths::Clamp(_event.wiL.y, (Real)-1, (Real)1));
	const Real thetaO = std::acos(maths::Clamp(_event.woL.y, (Real)-1, (Real)1));
	const Real alpha = std::max(thetaI, thetaO);
	const Real beta = std::min(thetaI, thetaO);
	const Real phiI = std::acos(CosPhi(_event.wiL));
	const Real phiO = std::acos(CosPhi(_event.woL));
	const Real sigma2 = _sigma * _sigma;
	const Real A = (Real)1 - sigma2 / ((Real)2 * (sigma2 + (Real).33));
	const Real B = ((Real).45 * sigma2) / (sigma2 + (Real).09);
	return A + B * std::max((Real)0, std::cos(phiI - phiO) * maths::Clamp(std::sin(alpha), (Real)-1, (Real)1) * maths::Clamp(std::tan(beta), (Real)-1, (Real)1));
}

Spectrum OrenNayarBRDF::f(const ScatterEvent &_event) const {
	const Real oN = OrenNayarTerm(_event, (*sigmaSocket)->GetAs<Real>(_event));
	const Spectrum albedoSpec = (*albedoSocket)->GetAsSpectrum(_event);
	return albedoSpec * INV_PI * oN;
}

Spectrum OrenNayarBRDF::Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	_event.wiL = Sampling::SampleCosineHemisphere(_sampler.Get2D());
	const bool isInside = _event.woL.y < 0;
	if (isInside) _event.wiL.y *= -1;
	_pdf = CosineHemispherePdf(_event.woL, _event.wiL);
	_event.wi = _event.ToWorld(_event.wiL);
	_event.hit->point += _event.hit->normalG * (isInside ? -SURFACE_EPSILON : SURFACE_EPSILON);
	return f(_event);
}



OrenNayarBTDF::OrenNayarBTDF(ShaderGraph::Socket **_albedo, ShaderGraph::Socket **_sigma) : BxDF((BxDFType)(BxDF_TRANSMISSION | BxDF_DIFFUSE)) {
	albedoSocket = _albedo;
	sigmaSocket = _sigma;
}

Spectrum OrenNayarBTDF::f(const ScatterEvent &_event) const {
	const Real oN = OrenNayarTerm(_event, (*sigmaSocket)->GetAs<Real>(_event));
	const Spectrum albedoSpec = (*albedoSocket)->GetAsSpectrum(_event);
	return albedoSpec * INV_PI * oN;
}

Spectrum OrenNayarBTDF::Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	_event.wiL = Sampling::SampleCosineHemisphere(_sampler.Get2D());
	const bool entering = _event.woL.y > 0;
	_pdf = CosineHemispherePdf(_event.woL, _event.wiL);
	if (entering) _event.wiL.y *= -1;
	_event.wi = _event.ToWorld(_event.wiL);
	_event.hit->point += _event.hit->normalG * (entering ? -SURFACE_EPSILON : SURFACE_EPSILON);
	return f(_event);
}

LAMBDA_END