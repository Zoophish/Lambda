#include "TextureEncoding.h"


LAMBDA_BEGIN

/*
	Based on: https://github.com/rawrunprotected/hilbert_curves
*/
uint32_t TextureEncoding::HilbertXYToIndex(uint32_t n, uint32_t x, uint32_t y) {
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

LAMBDA_END