#include "Microfacet.h"

LAMBDA_BEGIN

MicrofacetBRDF::MicrofacetBRDF(ShaderGraph::SocketRef *_albedoSocket, ShaderGraph::SocketRef *_roughnesSocket, MicrofacetDistribution *_distribution, Fresnel *_fresnel) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_GLOSSY)) {
	albedoSocket = _albedoSocket;
	roughnessSocket = _roughnesSocket;
	distribution = _distribution;
	fresnel = _fresnel;
}

Spectrum MicrofacetBRDF::f(const ScatterEvent &_event) const {
	const Real cosThetaO = std::abs(_event.woL.y), cosThetaI = std::abs(_event.wiL.y);
	Vec3 wh = _event.wiL + _event.woL;
	if (cosThetaI == 0 || cosThetaO == 0) return Spectrum(0);
	if (wh.x == 0 && wh.y == 0 && wh.z == 0) return Spectrum(0);
	wh = wh.Normalised();
	const Vec3 wiL(_event.wiL.x, maths::Clamp(_event.wiL.y, (Real)0, (Real)1), _event.wiL.z);
	const Spectrum F = fresnel->Evaluate(maths::Dot(wiL, wh), etaT);
	const Vec2 alpha = RoughnessToAlpha(_event);
	const Real D = distribution->D(wh, alpha);
	const Real G = distribution->G(_event.woL, wiL, alpha);
	const Spectrum albedoSpec = albedoSocket->GetAsSpectrum(_event);
	return albedoSpec * D * G * F / (4 * cosThetaI * cosThetaO);
}

Spectrum MicrofacetBRDF::Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	if (_event.woL.y == 0) return Spectrum(0);
	const Vec2 alpha = RoughnessToAlpha(_event);
	const Vec3 wh = distribution->Sample_wh(_sampler, _event.woL, alpha).Normalised();
	_event.wiL = Reflect(-_event.woL, wh);
	if (!SameHemisphere(_event.woL, _event.wiL)) {
		_pdf = 0;
		return Spectrum(0);
	}
	_pdf = distribution->Pdf(wh, alpha) / (4 * maths::Dot(_event.woL, wh));
	_event.wi = _event.ToWorld(_event.wiL);
	_event.hit->point += _event.hit->normalG * SURFACE_EPSILON;
	return f(_event);
}

Real MicrofacetBRDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const {
	if (!SameHemisphere(_wo, _wi)) return 0;
	const Vec3 wh = (_wo + _wi).Normalised();
	const Vec2 alpha = RoughnessToAlpha(_event);
	return distribution->Pdf(wh, alpha) / (4 * maths::Dot(_wo, wh));
}

static inline Real ToAlpha(Real _roughness) {
	_roughness = std::max(_roughness, (Real)1e-3);
	Real x = std::log(_roughness);
	x = (Real)1.62142 + (Real)0.819955 * x + (Real)0.1734 * x * x + (Real)0.0171201 * x * x * x + (Real)0.000640711 * x * x * x * x;
	return x;
}

inline Vec2 MicrofacetBRDF::RoughnessToAlpha(const ScatterEvent &_event) const {
	switch (roughnessSocket->socketType) {
		case ShaderGraph::SocketType::TYPE_SCALAR:	//Isotropic distribution
		{
			const Real roughness = ToAlpha(roughnessSocket->GetAs<Real>(_event));
			return Vec2(roughness, roughness);
		}
		case ShaderGraph::SocketType::TYPE_VEC2:	//Anisotropic distribution
		{
			const Vec2 roughness = roughnessSocket->GetAs<Vec2>(_event);
			return Vec2(ToAlpha(roughness.x), ToAlpha(roughness.y));
		}
	}
}

LAMBDA_END