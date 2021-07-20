#include "BxDF.h"

LAMBDA_BEGIN

BxDF::BxDF(const BxDFType _type) : type(_type) {}

Spectrum BxDF::Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	_event.wiL = Sampling::SampleCosineHemisphere(_sampler.Get2D());
	if (_event.woL.y < 0) _event.wiL.y *= -1;
	_pdf = CosineHemispherePdf(_event.woL, _event.wiL);
	_event.wi = _event.ToWorld(_event.wiL);
	_event.hit->point += _event.hit->normalG * SURFACE_EPSILON *_event.sidedness;
	return f(_event);
}

Spectrum BxDF::Rho(const ScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const {
	return Spectrum(0);
}

Real BxDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const {
	return CosineHemispherePdf(_wo, _wi);
}



LambertianBRDF::LambertianBRDF(ShaderGraph::SocketRef *_aledoSocket) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_DIFFUSE)) {
	albedoSocket = _aledoSocket;
}

Spectrum LambertianBRDF::f(const ScatterEvent &_event) const {
	return albedoSocket->GetAsSpectrum(_event) * INV_PI;
}

Spectrum LambertianBRDF::Rho(const ScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const {
	return albedoSocket->GetAsSpectrum(_event);
}



MixBSDF::MixBSDF(ShaderGraph::SocketRef *_aSocket, ShaderGraph::SocketRef *_bSocket, ShaderGraph::SocketRef *_ratioSocket) : BxDF(BxDF_DIFFUSE) {
	aSocket = _aSocket;
	bSocket = _bSocket;
	ratioSocket = _ratioSocket;
}

Spectrum MixBSDF::f(const ScatterEvent &_event) const {
	const Real ratio = ratioSocket->GetAs<Real>(_event);
	return aSocket->GetAs<BxDF *>(_event)->f(_event) * ((Real)1 - ratio) + bSocket->GetAs<BxDF *>(_event)->f(_event) * ratio;
}

Spectrum MixBSDF::Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	const Real ratio = ratioSocket->GetAs<Real>(_event);
	if (_sampler.Get1D() > ratio) {
		const Spectrum af = aSocket->GetAs<BxDF *>(_event)->Sample_f(_event, _sampler, _pdf);
		return af * ((Real)1 - ratio) + bSocket->GetAs<BxDF *>(_event)->f(_event) * ratio;
	}
	else { //use refs, bad code
		const Spectrum bf = bSocket->GetAs<BxDF *>(_event)->Sample_f(_event, _sampler, _pdf);
		return bf * ratio + aSocket->GetAs<BxDF *>(_event)->f(_event) * ((Real)1 - ratio);
	}
}

Real MixBSDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const {
	const Real ratio = ratioSocket->GetAs<Real>(_event);
	return aSocket->GetAs<BxDF *>(_event)->Pdf(_wo, _wi, _event) * ((Real)1 - ratio) + bSocket->GetAs<BxDF *>(_event)->Pdf(_wo, _wi, _event) * ratio;
}

LAMBDA_END