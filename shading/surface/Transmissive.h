#pragma once
#include "BxDF.h"

class TransmissiveBTDF : public BxDF {
	public:
		TextureAdapter alpha;

		TransmissiveBTDF(TextureR32 *_alpha) : BxDF(BxDF_TRANSMISSION) {
			alpha = _alpha;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return Spectrum(0);
		}

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override {
			_event.pdf = 1;
			_event.wi = -_event.wo;
			_event.wiL = _event.ToLocal(_event.wi);
			_event.hit->point += _event.wi * SURFACE_EPSILON;
			return alpha.GetUV(_event.hit->uvCoords) / std::abs(_event.wiL.y);
		}
};