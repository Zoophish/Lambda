#pragma once
#include <Lambda.h>
#include <maths/maths.h>

LAMBDA_BEGIN

struct FourierSpectrum {
	Real *frequencies, *powers;
	unsigned numFrequencies;

	~FourierSpectrum() {
		delete[] frequencies, powers;
	}
};

LAMBDA_END