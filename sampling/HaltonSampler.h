#pragma once
#include "Sampler.h"
#include "SampleShifter.h"

LAMBDA_BEGIN

class HaltonSampler : public Sampler {
	public:
		HaltonSampler(const unsigned _sampleIndex = 0);

		/*
			Makes an identical copy of this sampler (needed for parallel rendering).
		*/
		Sampler *clone() const override;

		/*
			Progresses sequence points to next sample.
		*/
		void NextSample() override;

		/*
			Sets the sequence to a specific sample index (i.e. for resuming renders).
		*/
		void SetSample(const unsigned _sampleIndex) override;

		/*
			Returns a 1D point in sample space of current sample. 
		*/
		Real Get1D() override;

		/*
			Returns a 2D point in sample space of current sample.
		*/
		Vec2 Get2D() override;

	protected:
		static const unsigned maxDimension = 7;
		static const unsigned primes[maxDimension];
		Real sequence[maxDimension];

	private:
		static inline Real Halton(const int index, const int b);
};

LAMBDA_END