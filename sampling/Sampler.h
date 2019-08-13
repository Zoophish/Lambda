#pragma once
#include <maths/maths.h>
#include "SampleShifter.h"
 
class Sampler {
	public:
		unsigned sampleIndex;

		SampleShifter *sampleShifter = nullptr;

		virtual void NextSample() = 0;

		virtual void SetSample(const unsigned _sampleIndex) = 0;

		virtual Real Get1D() = 0;
		
		virtual Vec2 Get2D() = 0;

	protected:
		unsigned dimensionIndex;
};