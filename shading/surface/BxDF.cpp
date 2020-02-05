#include "BxDF.h"

LAMBDA_BEGIN

BxDF::BxDF(const BxDFType _type) : type(_type) {}

Spectrum BxDF::Sample_f(SurfaceScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	_event.wiL = Sampling::SampleCosineHemisphere(_sampler.Get2D());
	if (_event.woL.y < 0) _event.wiL.y *= -1;
	_pdf = CosineHemispherePdf(_event.woL, _event.wiL);
	_event.pdf = _pdf;
	_event.wi = _event.ToWorld(_event.wiL);
	_event.hit->point += _event.hit->normalG * SURFACE_EPSILON;
	return f(_event);
}

Spectrum BxDF::Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const {
	return Spectrum(0);
}

Real BxDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const {
	return CosineHemispherePdf(_wo, _wi);
}



LambertianBRDF::LambertianBRDF(ShaderGraph::Socket **_aledoSocket) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_DIFFUSE)) {
	albedoSocket = _aledoSocket;
}

Spectrum LambertianBRDF::f(const SurfaceScatterEvent &_event) const {
	return (*albedoSocket)->GetAsSpectrum(&_event) * INV_PI;
}

Spectrum LambertianBRDF::Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const {
	return (*albedoSocket)->GetAsSpectrum(&_event);
}



MixBSDF::MixBSDF(ShaderGraph::Socket **_aSocket, ShaderGraph::Socket **_bSocket, ShaderGraph::Socket **_ratioSocket) : BxDF(BxDF_DIFFUSE) {
	aSocket = _aSocket;
	bSocket = _bSocket;
	ratioSocket = _ratioSocket;
}

Spectrum MixBSDF::f(const SurfaceScatterEvent &_event) const {
	const Real ratio = (*ratioSocket)->GetAsScalar(&_event);
	return (*aSocket)->GetAsBxDF(&_event)->f(_event) * ((Real)1 - ratio) + (*bSocket)->GetAsBxDF(&_event)->f(_event) * ratio;
}

Spectrum MixBSDF::Sample_f(SurfaceScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	const Real ratio = (*ratioSocket)->GetAsScalar(&_event);
	if (_sampler.Get1D() > ratio) {
		const Spectrum af = (*aSocket)->GetAsBxDF(&_event)->Sample_f(_event, _sampler, _event.pdf);
		_pdf = _event.pdf;
		return af * ((Real)1 - ratio) + (*bSocket)->GetAsBxDF(&_event)->f(_event) * ratio;
	}
	else { //use refs, bad code
		const Spectrum bf = (*bSocket)->GetAsBxDF(&_event)->Sample_f(_event, _sampler, _event.pdf);
		_pdf = _event.pdf;
		return bf * ratio + (*aSocket)->GetAsBxDF(&_event)->f(_event) * ((Real)1 - ratio);
	}
}

Real MixBSDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const {
	const Real ratio = (*ratioSocket)->GetAsScalar(&_event);
	return (*aSocket)->GetAsBxDF(&_event)->Pdf(_wo, _wi, _event) * ((Real)1 - ratio) + (*bSocket)->GetAsBxDF(&_event)->Pdf(_wo, _wi, _event) * ratio;
}

LAMBDA_END