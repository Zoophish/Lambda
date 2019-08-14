#pragma once
#include "BxDF.h"

class Emission : public BxDF {
	public:
		Spectrum emission;

		Emission(const Spectrum &_emission) : BxDF(BxDFType::EMISSION) {
			emission = _emission;
		}

		Spectrum f(const SurfaceScatterEvent &_event) const override {
			return emission;
		}
};