#include "BxDF.h"

BxDF::BxDF(const BxDFType _type) : type(_type) {}

Spectrum BxDF::Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const {
	_event.wiL = Sampling::SampleCosineHemisphere(_u);
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



LambertianBRDF::LambertianBRDF(Texture *_albedo) : BxDF((BxDFType)(BxDF_REFLECTION | BxDF_DIFFUSE)) {
	albedo.SetTexture(_albedo);
}

Spectrum LambertianBRDF::f(const SurfaceScatterEvent &_event) const {
	return albedo.GetUV(_event.hit->uvCoords) * INV_PI;
}

Spectrum LambertianBRDF::Rho(const SurfaceScatterEvent &_event, const unsigned _nSample, Vec2 *_smpls) const {
	return albedo.GetUV(_event.hit->uvCoords);
}



MixBSDF::MixBSDF(BxDF *_a, BxDF *_b, const Real _factor) : BxDF(BxDF_DIFFUSE) {
	a = _a;
	b = _b;
	factor = _factor;
}

Spectrum MixBSDF::f(const SurfaceScatterEvent &_event) const {
	return a->f(_event) * ((Real)1 - factor) + b->f(_event) * factor;
}