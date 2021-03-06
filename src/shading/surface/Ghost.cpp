#include "Ghost.h"

LAMBDA_BEGIN

GhostBTDF::GhostBTDF(ShaderGraph::SocketRef *_alphaSocket) : BxDF(BxDF_TRANSMISSION) {
	alphaSocket = _alphaSocket;
}

Spectrum GhostBTDF::f(const ScatterEvent &_event) const {
	return Spectrum(0);
}

Spectrum GhostBTDF::Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const {
	_pdf = 1;
	const Real alpha = alphaSocket->GetAs<Real>(_event);
	const bool isInside = _event.woL.y < 0;
	if (alpha > 0) {
		_pdf = 1;
		_pdf = 1;
		_event.wi = -_event.wo;
		_event.wiL = _event.ToLocal(_event.wi);
		_event.hit->point += _event.hit->normalG * SURFACE_EPSILON * 2 * (Real)(isInside ? 1 : -1);
		return Spectrum(alpha / std::abs(_event.wiL.y));
	}
	_pdf = 0;
	_pdf = 0;
	return Spectrum(0);
}

Real GhostBTDF::Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const {
	return 0;
}

LAMBDA_END