#pragma once
#include "ThreadCache.hpp"


void* ConcurrentAlloc(size_t size) { // 对每一个线程，用来统一进行内存分配
	if (pTLSThreadCache == nullptr) {
		pTLSThreadCache = new ThreadCache();
	}
	return pTLSThreadCache->Allocate(size);
}

void ConcurrentDealloc(void* ptr, size_t size) { // 对每一个线程，进行内存的释放
	assert(pTLSThreadCache);
	pTLSThreadCache->Deallocate(ptr, size);
	
}