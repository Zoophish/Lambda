#pragma once
#include <memory>
#include <list>
#include <algorithm>

constexpr unsigned L1_CACHE_LINE_SIZE = 64;	//64 bit cache line size
constexpr unsigned ALLOC_BLOCK_SIZE = 262144;	//256kb in bits

void *AllocAligned(const size_t _size);

template<class T>
inline T *AllocAligned(const size_t _count) {
	return (T *)AllocAligned(_count * sizeof(T));
}

void FreeAligned(void *_ptr);

class alignas(L1_CACHE_LINE_SIZE) MemoryArena {
	using byte_t = uint8_t;
	private:
		std::list<std::pair<size_t, byte_t *>> usedBlocks, availableBlocks;
		byte_t *currentBlock = nullptr;
		const size_t blockSize;
		size_t currentBlockPos = 0, currentAllocSize = 0;

		MemoryArena(const MemoryArena &) = delete;
		MemoryArena &operator=(const MemoryArena &) = delete;

	public:
		MemoryArena(const size_t _blockSize = ALLOC_BLOCK_SIZE);

		~MemoryArena();

		void *Alloc(size_t _nBytes);

		void Reset();

		template<class T, typename... params>
		inline T *New(params... _params) {
			return new((T *)Alloc(sizeof(T))) T(_params...);
		}

		template<class T>
		inline T *Alloc(const size_t _n = 1) {
			T *ret = (T *)Alloc(_n * sizeof(T));
			for (size_t i = 0; i < _n; ++i) &ret[i] = new T();
			return ret;
		}
};