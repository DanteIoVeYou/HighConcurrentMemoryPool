#pragma once
#include "common.hpp"


class ThreadCache { // ��װThreadCache�࣬���������ġ�С��256kb���ڴ�����
public:

	void* Allocate(size_t size) {

	}
	void Deallocate(void* ptr, size_t size) { //???

	}
private:

	FreeList _freeLists[11];

};