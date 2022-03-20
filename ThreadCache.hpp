#pragma once
#include "common.hpp"


class ThreadCache { // 封装ThreadCache类，管理无锁的、小于256kb的内存申请
public:

	void* Allocate(size_t size) {

	}
	void Deallocate(void* ptr, size_t size) { //???

	}
private:

	FreeList _freeLists[11];

};