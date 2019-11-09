#pragma once
#include <maths/maths.h>
class SampleShifter;

class Sampler {
	public:
		SampleShifter *sampleShifter = nullptr;

		virtual Sampler *clone() const = 0;

		virtual void NextSample() = 0;

		virtual void SetSample(const unsigned _sampleIndex) = 0;

		virtual Real Get1D() = 0;
		
		virtual Vec2 Get2D() = 0;

	protected:
		unsigned sampleIndex, dimensionIndex;
};