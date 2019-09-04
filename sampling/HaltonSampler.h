#pragma once
#include <vector>
#include "Sampler.h"

class HaltonSampler : public Sampler {
	public:
		HaltonSampler(const unsigned _sampleIndex = 0) {
			sampleIndex = _sampleIndex;
			sequence.resize(maxDimension);
			SetSample(0);
		}

		Sampler *clone() const override { return new HaltonSampler(*this); }

		void NextSample() override {
			for (unsigned i = 0; i < maxDimension; ++i) {
				sequence[i] = Halton(sampleIndex, primes[i]);
			}
			dimensionIndex = 0;
			++sampleIndex;
		}

		void SetSample(const unsigned _sampleIndex) override {
			sampleIndex = _sampleIndex;
			for (unsigned i = 0; i < maxDimension; ++i) {
				sequence[i] = Halton(sampleIndex, primes[i]);
			}
			dimensionIndex = 0;
		}

		Real Get1D() override {
			Real out;
			if (sampleShifter) {
				out = sampleShifter->Shift(sequence[dimensionIndex % maxDimension], dimensionIndex);
			}
			else { out = sequence[dimensionIndex % maxDimension]; }
			dimensionIndex++;
			return out;
		}

		Vec2 Get2D() override {
			Vec2 out;
			if (sampleShifter) {
				out =  Vec2(sampleShifter->Shift(sequence[dimensionIndex % maxDimension], dimensionIndex),
							sampleShifter->Shift(sequence[(dimensionIndex + 1) % maxDimension], dimensionIndex + 1));
			}
			else { out = Vec2(sequence[dimensionIndex % maxDimension], sequence[(dimensionIndex + 1) % maxDimension]); }
			dimensionIndex += 2;
			return out;

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