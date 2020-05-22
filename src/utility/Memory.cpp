#include "Memory.h"

void *AllocAligned(const size_t _size) {
	#if defined(_WIN32)
		return _aligned_malloc(_size, L1_CACHE_LINE_SIZE);
	#elif defined (__FreeBSD__) || defined(__APPLE__)
		void *ptr = nullptr;
		if (posix_memalign(&ptr, L1_CACHE_LINE_SIZE, _size) != 0)
			ptr = nullptr;
		return ptr;
	#else
		return memalign(L1_CACHE_LINE_SIZE, _size);
	#endif
}

void FreeAligned(void *_ptr) {
	if (!_ptr) return;
	#if defined(_WIN32)
		_aligned_free(_ptr);
	#else
		free(_ptr);
	#endif
}



MemoryArena::MemoryArena(const size_t _blockSize) : blockSize(_blockSize) {}

void *MemoryArena::Alloc(size_t _nBytes) {
	_nBytes = (_nBytes + 15) & (~15);
	if (currentBlockPos + _nBytes > currentAllocSize) {
		if (currentBlock) {
			usedBlocks.push_back(std::pair<size_t, byte_t *>(currentAllocSize, currentBlock));
			currentBlock = nullptr;
		}
		for (auto it = availableBlocks.begin(); it != availableBlocks.end(); ++it) {
			if (it->first >= _nBytes) {
				currentAllocSize = it->first;
				currentBlock = it->second;
				availableBlocks.erase(it);
				break;
			}
		}
		if (!currentBlock) {
			currentAllocSize = std::max(_nBytes, blockSize);
			currentBlock = AllocAligned<byte_t>(currentAllocSize);
		}
		currentBlockPos = 0;
	}
	void *ret = currentBlock + currentBlockPos;
	currentBlockPos += _nBytes;
	return ret;
}

void MemoryArena::Reset() {
	currentBlockPos = 0;
	availableBlocks.splice(availableBlocks.begin(), usedBlocks);
}

void MemoryArena::FreeAll() {
	Reset();
	for (auto &it : availableBlocks) FreeAligned(it.second);
	FreeAligned(currentBlock);
	currentAllocSize = 0;
	currentBlock = nullptr;
}