#pragma once
#include "BxDF.h"

class Emission : public BxDF {
	public:

		Emission() : BxDF(BxDFType::EMISSION) {}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return Spectrum(0);
		}
};