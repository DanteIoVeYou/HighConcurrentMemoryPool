#pragma once
#include "common.hpp"


class ThreadCache { // 封装ThreadCache类，管理无锁的、小于256KB的内存申请
public:
	void* FetchFromCentralCache(size_t size, size_t index);
	void* Allocate(size_t size);
	void Deallocate(void* ptr, size_t size);
	void FreeForListTooLong(FreeList& list, size_t size);

private:
	FreeList _freeLists[THREAD_CACHE_AND_CENTRAL_CACHE_HASH_BUCKET_SIZE];
};

// 静态绑定线程本地变量
static __declspec(thread) ThreadCache* pTLSThreadCache = nullptr;