#include "CentralCache.hpp"

CentralCache CentralCache::_singleInstance;
Span* CentralCache::GetOneSpan(SpanList& list, size_t size) {
	//TODO
	return nullptr;
}

size_t CentralCache::FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize) {
	// ����߳�ͬʱ��CentralCacheȡͬһ��SpanList�е�span�ڴ�����̲߳���ȫ��
	spanLists[index]._mtx.lock(); // ����
	Span* span = GetOneSpan(spanLists[index], size); // ���±�Ϊindex��Ͱ��SpanList��span��ȡbatchSize����С���ڴ��
	// ����ȡ����һ��
	assert(span != nullptr);
	start = span->_freeList;
	end = start;
	size_t actualBatchSize = 1;
	while (actualBatchSize <= batchSize && NextObj(end) != nullptr) {
		++actualBatchSize; // ������ҵ���һ���ڴ��
		end = NextObj(end);
	}
	end = nullptr;
	spanLists[index]._mtx.unlock(); // ����
	return actualBatchSize;
}
