#pragma once
#include <cstdint>
#include <Lambda.h>


LAMBDA_BEGIN

namespace TextureEncoding {

	inline uint64_t ShiftInterleave(const uint32_t _i) {
		uint64_t word = _i;
		word = (word ^ (word << 16)) & 0x0000ffff0000ffff;
		word = (word ^ (word << 8)) & 0x00ff00ff00ff00ff;
		word = (word ^ (word << 4)) & 0x0f0f0f0f0f0f0f0f;
		word = (word ^ (word << 2)) & 0x3333333333333333;
		word = (word ^ (word << 1)) & 0x5555555555555555;
		return word;
	}

	uint32_t HilbertXYToIndex(uint32_t n, uint32_t x, uint32_t y);
}

LAMBDA_END