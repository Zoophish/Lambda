#pragma once
#include "BxDF.h"

class TransparencyBTDF : public BxDF {
	public:
		TextureAdapter alpha;

		TransparencyBTDF(TextureR32 *_alpha) : BxDF(BxDF_TRANSMISSION) {
			alpha = _alpha;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return alpha.GetUV(_event.hit->uvCoords);
		}

		Spectrum Sample_f(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const override {
			_event.pdf = 1;
			_event.wi = -_event.wo;
			_event.hit->point -= _event.hit->normalG * SURFACE_EPSILON;
			return alpha.GetUV(_event.hit->uvCoords);
		}
};