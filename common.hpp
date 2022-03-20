#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>

static const int THREAD_CACHE_MAX_ALLOCATE_BYTES = 236 * 1024;
static const int Hash_BUCKET_SIZE = 208;



inline void*& NextObj(void* obj) { // ȡ��һ���ַ��ͷ��4/8�ֽڴ�ŵĵ�ַ
	return *(void**)obj;	
}


class FreeList
{
public:
	FreeList() 
		:_freeList(nullptr)
	{}
	void Push(void* obj) { // ͷ��
		assert(obj != nullptr); // ������nullptrʱ������
		NextObj(obj) = _freeList;
		_freeList = obj;
	}

	void* Pop() { // ͷɾ
		assert(_freeList != nullptr); // ��_freeList��һ���ڴ��Ҳû��ʱ������
		void* obj = _freeList;
		_freeList = NextObj(obj);
		return obj;
	}
	bool Empty() {
		return _freeList == nullptr;
	}
private:
	void* _freeList;
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

	// ��̬��Ա����û�����ص�thisָ�룬���ܷ��ʷǾ�̬��Ա(��Ա���� + ��Ա��������Ϊ������Ҫ�õ�thisָ��)
	static size_t RoundUp(size_t size) { // ���ع�ϣͰ���±�
		static size_t alignNums[5] = { 8,16,128, 1024, 8192 }; // ��ͬ�Ķ�����
		/*	�����ڴ��С(byte)		������(byye)	*/
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
			assert(false);
		}
	}
	
	static size_t Index(size_t size) { // �ṩ����ȡ�����ڴ����ֵ
		static size_t alignNums[5] = { 8,16,128, 1024, 8192 }; // ��ͬ�Ķ�����
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
			assert(false);
		}
	}


};


