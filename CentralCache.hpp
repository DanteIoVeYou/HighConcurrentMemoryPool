#pragma once
#include "common.hpp"

class CentralCache { // 单例模式
public:

	static CentralCache* GetSingleInstance() {
		return &_singleInstance;
	}
	size_t FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize); // 从index下标的桶的Span节点拿内存
	Span* GetOneSpan(SpanList& list, size_t size);
	SpanList spanLists[THREAD_CACHE_AND_CENTRAL_CACHE_HASH_BUCKET_SIZE]; // 哈希桶上挂链表
private:
	CentralCache() {} // 构造私有
	CentralCache(CentralCache& cc) = delete; // 禁拷贝构造
	CentralCache operator=(CentralCache& cc) = delete; // 禁赋值重载
	static CentralCache _singleInstance; // 单例
};
