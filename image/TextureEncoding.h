/*	---- Sam Warren 2019 ----
NOTE Morton and Hilbert will only work on textures with side lengths of 2^N.
Textures may be arranged differently in memory for various benefits.
	-	Scanline stores texels row by row and is required to save a texture to a file. (i.e. Use for render textures)
	-	Morton and Hilbert ordering can improve locality preservation between 2D->1D mapping. (i.e. Use for large material textures)
	-	Hilbert and Morton slightly outperform each other in different scenarios, however Morton is more practical.

	
Some newer processors have Binary Manipulation Instruction sets (BMI) that can be used to hardware accellerate Morton encoding/decoding.
Disable this definition if runtime errors occur.
*/
#define USE_BMI

#pragma once
#include <cstdint>
#include <immintrin.h>

namespace TextureEncoding {

	namespace {

		static const uint64_t x2_mask = 0xAAAAAAAAAAAAAAAA;
		static const uint64_t y2_mask = 0x5555555555555555;

		inline uint64_t ShiftInterleave(const uint32_t _i) {
			uint64_t word = _i;
			word = (word ^ (word << 16)) & 0x0000ffff0000ffff;
			word = (word ^ (word << 8)) & 0x00ff00ff00ff00ff;
			word = (word ^ (word << 4)) & 0x0f0f0f0f0f0f0f0f;
			word = (word ^ (word << 2)) & 0x3333333333333333;
			word = (word ^ (word << 1)) & 0x5555555555555555;
			return word;
		}

		/*
			Based on: https://github.com/rawrunprotected/hilbert_curves
		*/
		inline uint32_t HilbertXYToIndex(uint32_t n, uint32_t x, uint32_t y) {
			x = x << (16 - n);
			y = y << (16 - n);
			uint32_t A, B, C, D;
			{
				const uint32_t a = x ^ y;
				const uint32_t b = 0xFFFF ^ a;
				const uint32_t c = 0xFFFF ^ (x | y);
				const uint32_t d = x & (y ^ 0xFFFF);
				A = a | (b >> 1);
				B = (a >> 1) ^ a;
				C = ((c >> 1) ^ (b & (d >> 1))) ^ c;
				D = ((a & (c >> 1)) ^ (d >> 1)) ^ d;
			}
			{
				const uint32_t a = A;
				const uint32_t b = B;
				const uint32_t c = C;
				const uint32_t d = D;
				A = ((a & (a >> 2)) ^ (b & (b >> 2)));
				B = ((a & (b >> 2)) ^ (b & ((a ^ b) >> 2)));
				C ^= ((a & (c >> 2)) ^ (b & (d >> 2)));
				D ^= ((b & (c >> 2)) ^ ((a ^ b) & (d >> 2)));
			}
			{
				const uint32_t a = A;
				const uint32_t b = B;
				const uint32_t c = C;
				const uint32_t d = D;
				A = ((a & (a >> 4)) ^ (b & (b >> 4)));
				B = ((a & (b >> 4)) ^ (b & ((a ^ b) >> 4)));
				C ^= ((a & (c >> 4)) ^ (b & (d >> 4)));
				D ^= ((b & (c >> 4)) ^ ((a ^ b) & (d >> 4)));
			}
			{
				const uint32_t a = A;
				const uint32_t b = B;
				const uint32_t c = C;
				const uint32_t d = D;
				C ^= ((a & (c >> 8)) ^ (b & (d >> 8)));
				D ^= ((b & (c >> 8)) ^ ((a ^ b) & (d >> 8)));
			}
			const uint32_t a = C ^ (C >> 1);
			const uint32_t b = D ^ (D >> 1);
			const uint32_t i0 = x ^ y;
			const uint32_t i1 = b | (0xFFFF ^ (i0 | a));
			return ((ShiftInterleave(i1) << 1) | ShiftInterleave(i0)) >> (32 - 2 * n);
		}

	}

	/*
		Texture sides MUST be of 2^N.
	*/
	inline size_t MortonOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
		#ifdef USE_BMI
			return _pdep_u64(_y, y2_mask) | _pdep_u64(_x, x2_mask);
		#else
			return ShiftInterleave(_x) | (ShiftInterleave(_y) << 1);
		#endif
	}
	/*
		Texture size MUST be of 2^N and square.
	*/
	inline size_t HilbertOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
		return HilbertXYToIndex(_w, _x, _y);
	}

	/*
		Any texture size works.
	*/
	inline size_t ScanlineRowOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
		return _y * _w + _x;
	}

	/*
		Any texture size works.
	*/
	inline size_t ScanlineColOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
		return _x * _h + _y;
	}
}
