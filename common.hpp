#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>

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
private:
	void* _freeList;
};


class SizeClass {
public:
	static size_t RoundUp(size_t size) {
		/*	�����ڴ��С(byte)		������(byye)	*/
		/*	1-128					8			*/
		if(size < )
	}
private:

};
