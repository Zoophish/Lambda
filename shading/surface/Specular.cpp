#include "Specular.h"

FresnelBSDF::FresnelBSDF(ShaderGraph::Socket **_albedoSocket, ShaderGraph::Socket **_iorSocket) : BxDF((BxDFType)(BxDF_TRANSMISSION | BxDF_SPECULAR)) {
	albedoSocket = _albedoSocket;
	iorSocket = _iorSocket;
}

Spectrum FresnelBSDF::f(const SurfaceScatterEvent &_event) const {
	return Spectrum(0);
}

Spectrum FresnelBSDF::Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const {
	const bool entering = _event.woL.y > 0;
	const Real ior = (*iorSocket)->GetAsScalar(&_event);
	const Real a = entering ? _event.eta : ior;
	const Real b = entering ? ior : _event.eta;
	const Real fr = Fresnel::FrDielectric(_event.woL.y, a, b);
	if (_u.x < fr) {
		_event.wiL = Vec3(-_event.woL.x, _event.woL.y, -_event.woL.z);
		_event.wi = _event.ToWorld(_event.wiL);
		const Real cosTheta = std::abs(_event.wiL.y);
		_event.pdf = fr;
		_event.hit->point += _event.hit->normalG * SURFACE_EPSILON;
		return (*albedoSocket)->GetAsSpectrum(&_event) * fr / cosTheta;
	}
	else {
		const bool entering = _event.woL.y > 0;
		const Real etaI = a;
		const Real etaT = b;
		_event.pdf = 1 - fr;
		const bool refract = Refract(_event.woL, Vec3(0, 1, 0) * (entering ? 1 : -1), etaI / etaT, &_event.wiL);
		const Real cosTheta = std::abs(_event.wiL.y);
		_event.wi = _event.ToWorld(_event.wiL);
		Spectrum ft = (*albedoSocket)->GetAsSpectrum(&_event) * (1 - fr);
		if (refract) ft *= (etaI * etaI) / (etaT * etaT);
		_event.hit->point += _event.hit->normalG * (refract ? (entering ? -.001 : .001) : (entering ? .001 : -.001));
		return ft / cosTheta;
	}
}

Real FresnelBSDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const {
	return 0;
}



SpecularBRDF::SpecularBRDF(ShaderGraph::Socket **_albedoSocket, Fresnel *_fresnel) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_SPECULAR)) {
	fresnel = _fresnel;
	albedoSocket = _albedoSocket;
}

Spectrum SpecularBRDF::f(const SurfaceScatterEvent &_event) const {
	return Spectrum(0);
}

Spectrum SpecularBRDF::Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const {
	_event.pdf = 1;
	_pdf = 1;
	_event.wiL = Vec3(-_event.woL.x, _event.woL.y, -_event.woL.z);
	const Real cosTheta = _event.wiL.y;
	_event.wi = _event.ToWorld(_event.wiL);
	return (*albedoSocket)->GetAsSpectrum(&_event) / std::abs(cosTheta);
}



SpecularBTDF::SpecularBTDF(ShaderGraph::Socket **_albedoSocket, const Real _etaT) : BxDF((BxDFType)(BxDF_TRANSMISSION | BxDF_SPECULAR)) {
	albedoSocket = _albedoSocket;
	fresnel.etaT = _etaT;
}

Spectrum SpecularBTDF::f(const SurfaceScatterEvent &_event) const {
	return Spectrum(0);
}

Spectrum SpecularBTDF::Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const {
	_event.pdf = 1;
	const Vec3 woL = _event.ToLocal(_event.wo);
	const bool outside = woL.y > 0;
	_event.hit->normalS *= (outside ? 1 : -1);
	const Real etaI = outside ? _event.eta : fresnel.etaT;
	const Real etaT = outside ? fresnel.etaT : _event.eta;
	Refract(_event.wo, Vec3(0, 1, 0) * (outside ? 1 : -1), etaI / etaT, &_event.wi);
	Spectrum ft = (*albedoSocket)->GetAsSpectrum(&_event) * (Spectrum(1) - fresnel.Evaluate(std::abs(_event.wi.y), etaT));
	ft *= (etaI * etaI) / (etaT * etaT);
	const Real inv = (Real)1 / std::abs(_event.wi.y);
	_event.wi = _event.ToWorld(_event.wi);
	_event.hit->point += _event.hit->normalS * .0004;
	return ft * inv;
}

Real SpecularBTDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const {
	return 0;
}