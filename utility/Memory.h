#pragma once
#include <memory>
#include <list>
#include <algorithm>

#define L1_CACHE_LINE_SIZE 64	//64 bit cache line size
#define ALLOC_BLOCK_SIZE 262144	//256kb in bits

void *AllocAligned(const size_t _size);

template<class T>
inline T *AllocAligned(const size_t _count) {
	return (T *)AllocAligned(_count * sizeof(T));
}

void FreeAligned(void *_ptr);

class MemoryArena {
	using byte_t = uint8_t;
	private:
		const size_t blockSize;
		size_t currentBlockPos = 0, currentAllocSize = 0;
		byte_t *currentBlock = nullptr;
		std::list<std::pair<size_t, byte_t*>> usedBlocks, availableBlocks;

	public:
		MemoryArena(const size_t _blockSize = ALLOC_BLOCK_SIZE);

		void *Alloc(size_t _nBytes);

		void Reset();

		void FreeAll();

		template<class T, typename... Args>
		inline T *New(Args... _args) {
			return new((T *)Alloc(sizeof(T))) T(_args...);
		}

		template<class T>
		inline T *Alloc(const size_t _n = 1) {
			T *ret = (T *)Alloc(_n * sizeof(T));
			for (size_t i = 0; i < _n; ++i) &ret[i] = new T();
			return ret;
		}
};