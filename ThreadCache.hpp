#pragma once
#include "common.hpp"


class ThreadCache { // ��װThreadCache�࣬���������ġ�С��256kb���ڴ�����
public:
	void* FenchFromCentralCahce(size_t size, size_t index);
	void* Allocate(size_t size);
	void Deallocate(void* ptr, size_t size);
private:
	FreeList _freeLists[HASH_BUCKET_SIZE];
};

// ��̬���̱߳��ر���
static __declspec(thread) ThreadCache* pTLSThreadCache = nullptr;