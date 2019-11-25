#include "Ghost.h"

GhostBTDF::GhostBTDF(ShaderGraph::Socket **_alphaSocket) : BxDF(BxDF_TRANSMISSION) {
	alphaSocket = _alphaSocket;
}

Spectrum GhostBTDF::f(const SurfaceScatterEvent &_event) const {
	return Spectrum(0);
}

Spectrum GhostBTDF::Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const {
	_event.pdf = 1;
	const Spectrum out = (*alphaSocket)->GetAsSpectrum(&_event) / std::abs(_event.wiL.y);
	if (out[0] > 0) {
		_event.pdf = 1;
		_event.wi = -_event.wo;
		_event.wiL = _event.ToLocal(_event.wi);
		_event.hit->point += _event.wi * SURFACE_EPSILON;
		return out;
	}
	_event.pdf = 0;
	return out;
}