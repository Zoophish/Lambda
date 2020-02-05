#pragma once
#include <image/Texture.h>
#include <sampling/Piecewise.h>
#include <sampling/Sampler.h>
#include <sampling/Sampling.h>
#include <maths/maths.h>

LAMBDA_BEGIN

class Aperture {
	public:
		Sampler *sampler;
		Real size;

		/*
			Samples and returns a point on the aperture plane.
		*/
		virtual Vec2 Sample_p(Real *_pdf = nullptr) const = 0;
};

class BladeAperture : public Aperture {
	public:
		unsigned blades;

		BladeAperture(const unsigned _blades, const Real _size = 1);

		/*
			Samples and returns a point in the aperture polygon.
		*/
		Vec2 Sample_p(Real *_pdf = nullptr) const override;
};

class CircularAperture : public Aperture {
	public:
		CircularAperture(const Real _size);

		/*
			Samples and returns a point on the aperture disk.
		*/
		Vec2 Sample_p(Real *_pdf = nullptr) const override;
};

class MaskedAperture : public Aperture {
	public:
		MaskedAperture(Texture *_mask, Sampler *_sampler, const Real _size = 1);

		/*
			Samples and returns a point (proportional to mask) on the aperture plane.
		*/
		Vec2 Sample_p(Real *_pdf = nullptr) const override;

	protected:
		Texture *mask;
		Distribution::Piecewise2D maskDistribution;
		
		/*
			Generates sampling distibution for importnace sampling of aperture mask.
		*/
		void InitDistribution();
};

LAMBDA_END