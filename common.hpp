#pragma once

// 32位操作系统，有32个比特位数量的页号；64位操作系统，有64个比特位数量的页号
#ifdef _WIN64 // _Win64
typedef unsigned long long PAGE_SIZE;
#elif _WIN32 // _Win32
typedef size_t PAGE_SIZE;
#endif 



#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <thread>
#include <mutex>
static const int THREAD_CACHE_MAX_ALLOCATE_BYTES = 256 * 1024; // ThreadCache 一次分配给线程的最大内存
static const int HASH_BUCKET_SIZE = 208;


static void*& NextObj(void* obj) { // 取出一块地址的头上4/8字节存放的地址
	return *(void**)obj;	
}


class FreeList
{
public:
	FreeList() 
		:_freeList(nullptr)
	{}
	void Push(void* obj) { // 头插一个内存块
		assert(obj != nullptr); // 当插入nullptr时，断言
		NextObj(obj) = _freeList;
		_freeList = obj;
	}
	void PushRange(void* start, void* end) {
		NextObj(end) = _freeList;
		_freeList = start;
	}
	void* Pop() { // 头删
		assert(_freeList != nullptr); // 当_freeList上一个内存块也没挂时，断言
		void* obj = _freeList;
		_freeList = NextObj(obj);
		return obj;
	}
	bool Empty() {
		return _freeList == nullptr;
	}
private:
	void* _freeList;
public:
	size_t _MaxSize;
};


class SizeClass {
private:

	static inline size_t _Index(size_t size, size_t alignNum, size_t lastNum) {
		size_t returnIndex;
		if (size % alignNum == 0) {
			returnIndex = (size - lastNum) / alignNum;
		}
		else {
			returnIndex = (size - lastNum) / alignNum + 1;
		}
		return returnIndex;
	}
	static inline size_t _RoundUp(size_t size, size_t alignNum) {
		size_t returnSize;
		if (size % alignNum == 0) {
			returnSize = size;
		}
		else {
			returnSize = (size / alignNum + 1) * alignNum;
		}
		return returnSize;
	}
public:

	// 静态成员函数没有隐藏的this指针，不能访问非静态成员(成员函数 + 成员变量，因为访问需要用到this指针)
	static size_t RoundUp(size_t size) { // 返回哈希桶的下标
		static size_t alignNums[5] = { 8,16,128, 1024, 8192 }; // 不同的对齐数
		/*	申请内存大小(byte)		对齐数(byye)	*/
		/*	1-128					8			*/
		/*	128+1-128*8				16			*/
		/*	128*8+1-128*64			128			*/
		/*	128*64+1-128*512		1024		*/
		/*	128*512+1-128*2048		8192		*/
		if (size <= 128) {
			return _RoundUp(size, alignNums[0]);
		}
		else if (size <= 128 * 8) {
			return _RoundUp(size, alignNums[1]);
		}
		else if (size <= 128 * 64) {
			return _RoundUp(size, alignNums[2]);
		}
		else if (size <= 128 * 512) {
			return _RoundUp(size, alignNums[3]);
		}
		else if (size <= 128 * 2048) {
			return _RoundUp(size, alignNums[4]);
		}
		else {
			return -1; // size超过允许的最大值 THREAD_CACHE_MAX_ALLOCATE_BYTES
		}
	}
	
	static size_t Index(size_t size) { // 提供向上取整的内存对齐值
		static size_t alignNums[5] = { 8,16,128, 1024, 8192 }; // 不同的对齐数
		static size_t indexDivivors[4] = { 16, 56, 56, 56 };
		static size_t sizeGroups[4] = { 128, 1024, 8192, 65536 };
		if (size <= 128) {
			return _Index(size, alignNums[0], 0);
		}
		else if (size <= 128 * 8) {
			return _Index(size, alignNums[1], sizeGroups[0]) + indexDivivors[0];
		}
		else if (size <= 128 * 8) {
			return _Index(size, alignNums[2], sizeGroups[1]) + indexDivivors[0] + indexDivivors[1];
		}
		else if (size <= 128 * 8 * 8 * 8) {
			return _Index(size, alignNums[3], sizeGroups[0]) + indexDivivors[0] + indexDivivors[1] + indexDivivors[2];
		}
		else if (size <= 128 * 8 * 8 * 4) {
			return _Index(size, alignNums[4], sizeGroups[0]) + indexDivivors[0] + indexDivivors[1] + indexDivivors[2] + indexDivivors[3];
		}
		else {
			return -1; // size超过允许的最大值 THREAD_CACHE_MAX_ALLOCATE_BYTES
		}
	}
	static size_t ApplyBatchSizeFromCentralCache(size_t size) {
		if (size == 0) {
			return 0;
		}
		size_t batchSize = THREAD_CACHE_MAX_ALLOCATE_BYTES / size;
		if (batchSize < 2) {
			batchSize = 2;
		}
		else if (batchSize > 256) {
			batchSize = 256;
		}
		return batchSize;
	}

};



struct Span {
	PAGE_SIZE _pageID = 0;
	size_t _usedCount = 0;
	Span* _next = nullptr;
	Span* _prev = nullptr;
	void* _freeList = nullptr;
};

class SpanList {
public:
	void Insert(Span* pos, Span* newSpan) { // 在名为pos的Span后面插入新的Span
		assert(newSpan);
		assert(pos);
		pos->_next->_prev = newSpan;
		newSpan->_prev = pos;
		newSpan->_next = pos->_next;
		pos->_next = newSpan;
	}
	void Erase(Span* pos) {
		assert(pos);
		assert(_head != pos);
		Span* next = pos->_next;
		Span* prev = pos->_prev;
		prev->_next = next;
		next->_prev = prev;
	}
private:
	Span* _head = nullptr; // 哨兵位
public:
	std::mutex _mtx; // 桶锁
};