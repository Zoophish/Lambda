/*
Used to decorrelate pixels' sample sets, which converts aliasing artifacts into noise.
For the first maskDimensions of a sample set, a mask texture (e.g. blue noise) can be used dither the
	samples which can provide a cleaer image at low sample counts. After these dimensions are used,
	random toroidal shifts are applied.
*/

#pragma once
#include <maths/maths.h>
#include <image/Texture.h>

class SampleShifter {
	public:
		unsigned maxDimension = 1024;
		TextureRGBA32 *mask;

		SampleShifter(TextureRGBA32 *_mask = nullptr, const unsigned _pixelIndex = 0) {
			pixelIndex = _pixelIndex;
			mask = _mask;
		}

		inline void SetPixelIndex(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
			if (mask)
				pixelIndex = (_y % mask->GetHeight()) * mask->GetWidth() + (_x % mask->GetWidth());
			else
				pixelIndex = _y * _w + _x;
		}

		Real Shift(const Real _point, const unsigned _dimensionIndex) const {
			Real tmp = _point;
			if (mask && _dimensionIndex < maskDimensions) {
				ToroidalShift(tmp, (*mask)[pixelIndex][_dimensionIndex]);
			}
			else {
				ToroidalShift(tmp, (Real)(Hash(pixelIndex * maxDimension + _dimensionIndex) * inv32));
			}
			return tmp;
		}

	protected:
		static constexpr unsigned maskDimensions = 4;
		static constexpr double inv32 = 1.0 / 4294967296.0;
		unsigned pixelIndex;

		inline void ToroidalShift(Real &_point, const Real _shift) const {
			_point += _shift;
			if (_point >= 1.) 
				_point -= (Real)1;
		}

		inline uint32_t Hash(uint32_t a) const {
			a = (a + 0x7ed55d16) + (a << 12);
			a = (a ^ 0xc761c23c) ^ (a >> 19);
			a = (a + 0x165667b1) + (a << 5);
			a = (a + 0xd3a2646c) ^ (a << 9);
			a = (a + 0xfd7046c5) + (a << 3);
			a = (a ^ 0xb55a4f09) ^ (a >> 16);
			return a;
		}
};