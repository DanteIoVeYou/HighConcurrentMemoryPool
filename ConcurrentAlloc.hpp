#pragma once
#include "ThreadCache.hpp"


void* ConcurrentAlloc(size_t size) { // ��ÿһ���̣߳�����ͳһ�����ڴ����
	if (pTLSThreadCache == nullptr) {
		pTLSThreadCache = new ThreadCache();
	}
	return pTLSThreadCache->Allocate(size);
}

void ConcurrentDealloc(void* ptr, size_t size) { // ��ÿһ���̣߳������ڴ���ͷ�
	assert(pTLSThreadCache);
	pTLSThreadCache->Deallocate(ptr, size);
	
}