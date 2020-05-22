#pragma once
#include "Media.h"

LAMBDA_BEGIN

class HomogeneousMedium : public Medium {
	public:
		Spectrum sigmaA, sigmaS;

		HomogeneousMedium(const Spectrum &_sigmaA, const Spectrum &_sigmaS);

		/*
			Beam transmittance (attenuation) through a ray segment in the volume.
		*/
		Spectrum Tr(const Ray &_ray, const Real _tFar, Sampler &_sampler) const override;

		/*
			Samples a ray intersection within the medium for either a volume or surface interaction (_event.mediumInteraction).
			Provides the respective beam transmittance of the interaction.
		*/
		Spectrum Sample(const Ray &_ray, Sampler &_sampler, ScatterEvent &_event) const override;
};

LAMBDA_END