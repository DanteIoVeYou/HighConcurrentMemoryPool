#define _CRT_SECURE_NO_WARNINGS 1
#include "ThreadCache.hpp"

void* ThreadCache::Allocate(size_t size) {
	size_t roundUpSize = SizeClass::RoundUp(size);
	size_t index = SizeClass::Index(size);
	if (_freeLists[index].Empty()) {
		// ...
	}
	else {
		return _freeLists[index].Pop();
	}
}

void ThreadCache::Deallocate(void* ptr, size_t size) {
	
}