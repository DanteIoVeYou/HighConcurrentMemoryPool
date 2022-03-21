#pragma once
#include "common.hpp"

class CentralCache { // ����ģʽ
public:

	static CentralCache* GetSingleInstance() {
		return &_singleInstance;
	}
	size_t FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize);
	Span* GetOneSpan(SpanList& list, size_t size);
private:
	CentralCache() {} // ����˽��
	CentralCache(CentralCache& cc) = delete; // ����������
	CentralCache operator=(CentralCache& cc) = delete; // ����ֵ����
	SpanList spanLists[HASH_BUCKET_SIZE];
	static CentralCache _singleInstance;
};
//class CentralCache
//{
//public:
//	static CentralCache* GetSingleInstance()
//	{
//		return &_singleInstance;
//	}
//
//	// ��ȡһ���ǿյ�span
//	Span* GetOneSpan(SpanList& list, size_t size);
//
//	// �����Ļ����ȡһ�������Ķ����thread cache
//	size_t FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize);
//
//private:
//	SpanList spanLists[HASH_BUCKET_SIZE];
//
//private:
//	CentralCache()
//	{}
//
//	CentralCache(const CentralCache&) = delete;
//
//	static CentralCache _singleInstance;
//};