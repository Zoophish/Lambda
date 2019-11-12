#include "Ghost.h"

GhostBTDF::GhostBTDF(TextureR32 *_alpha) : BxDF(BxDF_TRANSMISSION) {
	alpha = _alpha;
}

Spectrum GhostBTDF::f(const SurfaceScatterEvent &_event) const {
	return Spectrum(0);
}

Spectrum GhostBTDF::Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const {
	_event.pdf = 1;
	const Spectrum out = alpha.GetUV(_event.hit->uvCoords) / std::abs(_event.wiL.y);
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