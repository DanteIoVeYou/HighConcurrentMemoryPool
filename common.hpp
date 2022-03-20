#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>

inline void*& NextObj(void* obj) { // 取出一块地址的头上4/8字节存放的地址
	return *(void**)obj;	
}


class FreeList
{
public:
	FreeList() 
		:_freeList(nullptr)
	{}
	void Push(void* obj) { // 头插
		assert(obj != nullptr); // 当插入nullptr时，断言
		NextObj(obj) = _freeList;
		_freeList = obj;
	}

	void* Pop() { // 头删
		assert(_freeList != nullptr); // 当_freeList上一个内存块也没挂时，断言
		void* obj = _freeList;
		_freeList = NextObj(obj);
		return obj;
	}
private:
	void* _freeList;
};


class SizeClass {
public:
	static size_t RoundUp(size_t size) {
		/*	申请内存大小(byte)		对齐数(byye)	*/
		/*	1-128					8			*/
		if(size < )
	}
private:

};
