#include "Specular.h"

LAMBDA_BEGIN

FresnelBSDF::FresnelBSDF(ShaderGraph::SocketRef *_albedoSocket, ShaderGraph::SocketRef *_iorSocket) : BxDF((BxDFType)(BxDF_TRANSMISSION | BxDF_SPECULAR)) {
	albedoSocket = _albedoSocket;
	iorSocket = _iorSocket;
}

Spectrum FresnelBSDF::f(const ScatterEvent &_event) const {
	return Spectrum(0);
}

static inline Vec3 FaceForward(const Vec3 &_n, const Vec3 &_v) {
	return (maths::Dot(_n, _v) < (Real)0) ? -_n : _n;
}

Spectrum FresnelBSDF::Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	const bool entering = _event.woL.y > 0;

	const Real ior = iorSocket->GetAs<Real>(_event);
	
	const Real fr = Fresnel::FrDielectric(_event.woL.y, 1, ior);
	if (_sampler.Get1D() < fr) {
		_event.wiL = Vec3(-_event.woL.x, _event.woL.y, -_event.woL.z);
		_event.wi = _event.ToWorld(_event.wiL);
		const Real cosTheta = std::abs(_event.wiL.y);
		_event.hit->point += _event.hit->normalG * (entering ? SURFACE_EPSILON : -SURFACE_EPSILON);
		_pdf = fr;
		return albedoSocket->GetAsSpectrum(_event) * fr / cosTheta;
	}
	else {
		const Real etaI = _event.eta = entering ? 1 : ior;
		const Real etaT = entering ? ior : 1;
		const bool refract = Refract(_event.woL, FaceForward(Vec3(0, 1, 0), _event.woL), etaI / etaT, &_event.wiL);
		if (!refract) return 0;
		Spectrum ft = albedoSocket->GetAsSpectrum(_event) * (1 - fr);
		if (refract) ft *= (etaI * etaI) / (etaT * etaT);
		const Real cosTheta = std::abs(_event.wiL.y);
		_event.wi = _event.ToWorld(_event.wiL);
		_event.hit->point += _event.hit->normalG *(entering ? -SURFACE_EPSILON * 2 : SURFACE_EPSILON * 2);
		_pdf = (Real)1 - fr;
		return ft / cosTheta;
	}
}

Real FresnelBSDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const {
	return 0;
}



SpecularBRDF::SpecularBRDF(ShaderGraph::SocketRef *_albedoSocket, Fresnel *_fresnel) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_SPECULAR)) {
	fresnel = _fresnel;
	albedoSocket = _albedoSocket;
}

Spectrum SpecularBRDF::f(const ScatterEvent &_event) const {
	return Spectrum(0);
}

Spectrum SpecularBRDF::Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	_pdf = 1;
	_event.wiL = Vec3(-_event.woL.x, _event.woL.y, -_event.woL.z);
	_event.wi = _event.ToWorld(_event.wiL);
	_event.hit->point += _event.hit->normalG * SURFACE_EPSILON;
	return albedoSocket->GetAsSpectrum(_event) / std::abs(_event.wiL.y);
}



SpecularBTDF::SpecularBTDF(ShaderGraph::SocketRef *_albedoSocket, const Real _etaT) : BxDF((BxDFType)(BxDF_TRANSMISSION | BxDF_SPECULAR)) {
	albedoSocket = _albedoSocket;
	fresnel.etaT = _etaT;
}

Spectrum SpecularBTDF::f(const ScatterEvent &_event) const {
	return Spectrum(0);
}

Spectrum SpecularBTDF::Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	_pdf = 1;
	const Vec3 woL = _event.ToLocal(_event.wo);
	const bool outside = woL.y > 0;
	_event.hit->normalS *= (outside ? 1 : -1);
	const Real etaI = outside ? _event.eta : fresnel.etaT;
	const Real etaT = outside ? fresnel.etaT : _event.eta;
	Refract(_event.wo, Vec3(0, 1, 0) * (outside ? 1 : -1), etaI / etaT, &_event.wiL);
	Spectrum ft = albedoSocket->GetAsSpectrum(_event) * (Spectrum(1) - fresnel.Evaluate(std::abs(_event.wi.y), etaT));
	ft *= (etaI * etaI) / (etaT * etaT);
	const Real inv = (Real)1 / std::abs(_event.wiL.y);
	_event.wi = _event.ToWorld(_event.wiL);
	_event.hit->point += _event.hit->normalS * .0004;
	return ft * inv;
}

Real SpecularBTDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const {
	return 0;
}

LAMBDA_END