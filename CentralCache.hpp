#pragma once
#include "common.hpp"

class CentralCache { // ����ģʽ
public:

	static CentralCache* GetSingleInstance() {
		return &_singleInstance;
	}
	size_t FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize); // ��index�±��Ͱ��Span�ڵ����ڴ�
	Span* GetOneSpan(SpanList& list, size_t size);
	SpanList spanLists[THREAD_CACHE_AND_CENTRAL_CACHE_HASH_BUCKET_SIZE]; // ��ϣͰ�Ϲ�����
private:
	CentralCache() {} // ����˽��
	CentralCache(CentralCache& cc) = delete; // ����������
	CentralCache operator=(CentralCache& cc) = delete; // ����ֵ����
	static CentralCache _singleInstance; // ����
};
