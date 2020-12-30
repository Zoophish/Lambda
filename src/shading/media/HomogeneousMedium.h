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
		Spectrum SampleDistance(const Ray &_ray, Sampler &_sampler, ScatterEvent &_event, Real *_t, Real *_pdf) const override;

		/*
			Probability of sampling distance _t along ray segment.
		*/
		Real PDFDistance(const Real _t) const override;


		/*
			Samples t along ray segment within medium proportionally to the contribution from _lightPoint.
		*/
		Spectrum SampleEquiangular(const Ray &_ray, Sampler &_sampler, ScatterEvent &_event, const Vec3 &_lightPoint, Real *_t, Real *_pdf) const override;

		/*
			Probability of sampling distance _t along ray segment from _lightPoint.
		*/
		Real PDFEquiangular(const Ray &_ray, const Vec3 &_lightPoint, const Real _tFar, const Real _t) const override;
};

LAMBDA_END