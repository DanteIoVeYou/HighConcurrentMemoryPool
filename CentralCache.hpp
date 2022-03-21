#pragma once
#include "common.hpp"

class CentralCache { // ����ģʽ
public:
	static CentralCache* GetSingleInstance();
	void FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize);
private:
	CentralCache() {} // ����˽��
	CentralCache(CentralCache& cc) = delete; // ����������
	CentralCache operator=(CentralCache& cc) = delete; // ����ֵ����
	SpanList spanLists[HASH_BUCKET_SIZE];
	static CentralCache _singleInstance;
};