#pragma once
#include "common.hpp"


class ThreadCache { // ��װThreadCache�࣬���������ġ�С��256KB���ڴ�����
public:
	void* FetchFromCentralCache(size_t size, size_t index);
	void* Allocate(size_t size);
	void Deallocate(void* ptr, size_t size);
	void FreeForListTooLong(FreeList& list, size_t size);

private:
	FreeList _freeLists[THREAD_CACHE_AND_CENTRAL_CACHE_HASH_BUCKET_SIZE];
};

// ��̬���̱߳��ر���
static __declspec(thread) ThreadCache* pTLSThreadCache = nullptr;