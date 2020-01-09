/*
Used to change the correlation of pixels' sample sets, which converts aliasing artifacts into noise.
For the first maskDimensions of a sample set, a mask texture's (e.g. blue noise) channels can be used dither the
	samples which can provide a cleaner image at low sample counts. After these dimensions are used,
	a random hash is used.
*/

#pragma once
#include <maths/maths.h>
#include <image/Texture.h>

class SampleShifter {
	public:
		unsigned maxDimension = 1024;
		unsigned maskDimensionStart = 1;
		Texture *mask;

		SampleShifter(Texture *_mask, const unsigned _pixelIndex = 0) {
			pixelIndex = _pixelIndex;
			mask = _mask;
		}

		/*
			Specifies which pixel in the image the sample shifter is using.
		*/
		inline void SetPixelIndex(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
			if (mask) {
				const unsigned xp = _x % mask->GetWidth();
				const unsigned yp = _y % mask->GetHeight();
				pixelIndex = yp * mask->GetWidth() + xp;
			}
			else
				pixelIndex = _y * _w + _x;
		}

		/*
			Shift the sample dimension, _point according to current pixelIndex. 
		*/
		Real Shift(const Real _point, const unsigned _dimensionIndex) const;

	private:
		static const unsigned maskDimensions = 4;
		static constexpr double inv32 = 1. / 4294967296.;
		unsigned pixelIndex;

		static inline void ToroidalShift(Real &_point, const Real _shift);

		static inline uint32_t Hash(uint32_t a);
};