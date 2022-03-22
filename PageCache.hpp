#pragma once
#include "common.hpp"

class PageCache {
public:
	static PageCache* GetInstance() {
		return &_singleInstance;
	}
	Span* NewSpan(size_t pageSize); // ��ȡһ��pageSizeҳ��С��Span
	std::mutex _mtx; // PageCache��������Ͱ������Ϊ����128��ҳ��Ͱʱ��ȡ�ݹ鸴��
private:
	PageCache(){}
	PageCache(const PageCache& pc) = delete;
	PageCache operator=(const PageCache& pc) = delete;
	static PageCache _singleInstance;
	SpanList _spanLists[PAGE_CACHE_HASH_BUCKET_SIZE]; // Span������������ţ�1��-128�ţ�8kb��ҳ
};