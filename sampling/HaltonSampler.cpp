#include "HaltonSampler.h"

LAMBDA_BEGIN

const unsigned HaltonSampler::primes[maxDimension] = { 2,3,5,7,11,13,17 };

HaltonSampler::HaltonSampler(const unsigned _sampleIndex) {
	sampleIndex = _sampleIndex;
	SetSample(0);
}

Sampler *HaltonSampler::clone() const {
	return new HaltonSampler(*this);
}

void HaltonSampler::NextSample() {
	for (unsigned i = 0; i < maxDimension; ++i) {
		sequence[i] = Halton(sampleIndex, primes[i]);
	}
	dimensionIndex = 0;
	++sampleIndex;
}

void HaltonSampler::SetSample(const unsigned _sampleIndex) {
	sampleIndex = _sampleIndex;
	for (unsigned i = 0; i < maxDimension; ++i) {
		sequence[i] = Halton(sampleIndex, primes[i]);
	}
	dimensionIndex = 0;
}

Real HaltonSampler::Get1D() {
	Real out;
	if (sampleShifter) {
		out = sampleShifter->Shift(sequence[dimensionIndex % maxDimension], dimensionIndex);
	}
	else { out = sequence[dimensionIndex % maxDimension]; }
	dimensionIndex++;
	return out;
}

Vec2 HaltonSampler::Get2D() {
	Vec2 out;
	if (sampleShifter) {
		out = Vec2(sampleShifter->Shift(sequence[dimensionIndex % maxDimension], dimensionIndex),
			sampleShifter->Shift(sequence[(dimensionIndex + 1) % maxDimension], dimensionIndex + 1));
	}
	else { out = Vec2(sequence[dimensionIndex % maxDimension], sequence[(dimensionIndex + 1) % maxDimension]); }
	dimensionIndex += 2;
	return out;
}

inline Real HaltonSampler::Halton(const int index, const int b) {
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

LAMBDA_END