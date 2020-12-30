#pragma once
#include <cstdint>
#include <Lambda.h>

LAMBDA_BEGIN

/*
	Stores large arrays of booleans more compactly. Size is multiple of 64-bits.
*/
class BoolArray {
	public:
		BoolArray() {
			arr = nullptr;
		}

		BoolArray(const int _size) {
			arr = new uint64_t[(_size + 63) / 64];
		}

		BoolArray(bool *_arr, const int _size) {
			arr = new uint64_t[(_size + 63) / 64];
			for (int i = 0; i < _size; ++i) {
				Set(i, _arr[i]);
			}
		}

		~BoolArray() {
			delete[] arr;
		}

		inline int operator[](const int _index) const {
			return arr[_index / 64] & 1 << (_index % 64);
		}

		inline int Get(const int _index) const {
			return arr[_index / 64] & 1 << (_index % 64);
		}

		inline void Set(const int _index, const bool _b) {
			arr[_index / 64] = _b ?
				arr[_index / 64] | 1 << (_index % 64) :
				arr[_index / 64] & ~(uint64_t)(1 << (_index % 64));
		}

	private:
		uint64_t *arr;
};

LAMBDA_END