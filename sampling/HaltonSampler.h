#pragma once
#include <vector>
#include "Sampler.h"

class HaltonSampler : public Sampler {
	public:
		HaltonSampler(const unsigned _sampleIndex = 0) {
			sampleIndex = _sampleIndex;
			sequence.resize(maxDimension);
			NextSample();
		}

		void NextSample() override {
			for (short i = 0; i < maxDimension; ++i) {
				sequence[i] = Halton(sampleIndex, primes[i]);
			}
			dimensionIndex = 0;
			++sampleIndex;
		}

		Real Get1D() override {
			if (sampleShifter) {
				return sampleShifter->Shift(sequence[dimensionIndex++ % maxDimension], dimensionIndex);
			}
			return sequence[dimensionIndex++ % maxDimension];
		}

		Vec2 Get2D() override {
			if (sampleShifter) {
				return Vec2(sampleShifter->Shift(sequence[dimensionIndex++ % maxDimension], dimensionIndex),
							sampleShifter->Shift(sequence[dimensionIndex++ % maxDimension], dimensionIndex));
			}
			return Vec2(sequence[dimensionIndex++ % maxDimension], sequence[dimensionIndex++ % maxDimension]);
		}

	protected:
		static const unsigned maxDimension = 7;
		static constexpr unsigned primes[maxDimension] = { 2,3,5,7,11,13,17 };
		std::vector<Real> sequence;

		inline Real Halton(const int index, const int b) const {
			Real result = 0;
			Real f = 1. / (Real)b;
			int i = index;
			while (i > 0) {
				result = result + f * (i % b);
				i /= b;
				f /= (Real)b;
			}
			return result;
		}
};