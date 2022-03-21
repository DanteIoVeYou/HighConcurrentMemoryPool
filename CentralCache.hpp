#pragma once
#include "common.hpp"

class CentralCache { // 单例模式
public:

	static CentralCache* GetSingleInstance() {
		return &_singleInstance;
	}
	size_t FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize);
	Span* GetOneSpan(SpanList& list, size_t size);
private:
	CentralCache() {} // 构造私有
	CentralCache(CentralCache& cc) = delete; // 禁拷贝构造
	CentralCache operator=(CentralCache& cc) = delete; // 进赋值重载
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
//	// 获取一个非空的span
//	Span* GetOneSpan(SpanList& list, size_t size);
//
//	// 从中心缓存获取一定数量的对象给thread cache
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