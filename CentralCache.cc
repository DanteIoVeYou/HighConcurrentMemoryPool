#include "CentralCache.hpp"

CentralCache CentralCache::_singleInstance;
Span* CentralCache::GetOneSpan(SpanList& list, size_t size) {
	//TODO
	return nullptr;
}

size_t CentralCache::FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize) {
	// 多个线程同时从CentralCache取同一个SpanList中的span内存块是线程不安全的
	spanLists[index]._mtx.lock(); // 加锁
	Span* span = GetOneSpan(spanLists[index], size); // 从下标为index的桶的SpanList的span中取batchSize个大小的内存块
	// 至少取到了一个
	assert(span != nullptr);
	start = span->_freeList;
	end = start;
	size_t actualBatchSize = 1;
	while (actualBatchSize <= batchSize && NextObj(end) != nullptr) {
		++actualBatchSize; // 又向后找到了一个内存块
		end = NextObj(end);
	}
	end = nullptr;
	spanLists[index]._mtx.unlock(); // 解锁
	return actualBatchSize;
}
