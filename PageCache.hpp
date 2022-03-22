#pragma once
#include "common.hpp"

class PageCache {
public:
	static PageCache* GetInstance() {
		return &_singleInstance;
	}
	Span* NewSpan(size_t pageSize); // 获取一个pageSize页大小的Span
	std::mutex _mtx; // PageCache不建议用桶锁，因为申请128个页的桶时会取递归复用
private:
	PageCache(){}
	PageCache(const PageCache& pc) = delete;
	PageCache operator=(const PageCache& pc) = delete;
	static PageCache _singleInstance;
	SpanList _spanLists[PAGE_CACHE_HASH_BUCKET_SIZE]; // Span的里包含若干张（1张-128张）8kb的页
};