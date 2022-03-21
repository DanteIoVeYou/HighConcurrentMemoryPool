#pragma once
#include "common.hpp"

class CentralCache { // 单例模式
public:
	static CentralCache* GetSingleInstance();
	void FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize);
private:
	CentralCache() {} // 构造私有
	CentralCache(CentralCache& cc) = delete; // 禁拷贝构造
	CentralCache operator=(CentralCache& cc) = delete; // 进赋值重载
	SpanList spanLists[HASH_BUCKET_SIZE];
	static CentralCache _singleInstance;
};