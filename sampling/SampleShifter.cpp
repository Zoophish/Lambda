#include "SampleShifter.h"

Real SampleShifter::Shift(const Real _point, const unsigned _dimensionIndex) const {
	Real tmp = _point;
	const int dim = _dimensionIndex - maskDimensionStart;
	if (mask && dim >= 0 && dim < maskDimensions) {
		ToroidalShift(tmp, (*mask)[pixelIndex][dim]);
	}
	else {
		ToroidalShift(tmp, (Real)(Hash(pixelIndex * maxDimension + _dimensionIndex) * inv32));
	}
	return tmp;
}

inline void SampleShifter::ToroidalShift(Real &_point, const Real _shift) {
	_point += _shift;
	if (_point >= 1.)
		_point -= (Real)1;
}

inline uint32_t SampleShifter::Hash(uint32_t a) {
	a = (a + 0x7ed55d16) + (a << 12);
	a = (a ^ 0xc761c23c) ^ (a >> 19);
	a = (a + 0x165667b1) + (a << 5);
	a = (a + 0xd3a2646c) ^ (a << 9);
	a = (a + 0xfd7046c5) + (a << 3);
	a = (a ^ 0xb55a4f09) ^ (a >> 16);
	return a;
}