#include "PageCache.hpp"
#include "CentralCache.hpp"
PageCache PageCache::_singleInstance;
Span* PageCache::NewSpan(size_t pageSize) {
	assert(pageSize >= 1 && pageSize <= PAGE_CACHE_HASH_BUCKET_SIZE);


	if (!_spanLists[pageSize - 1].Empty()) {
		return _spanLists[pageSize-1].PopFront();
	}

	// pageSize-1下标的桶无Span存货，从pageSize开始找有无桶有存货
	for (size_t i = pageSize; i < PAGE_CACHE_HASH_BUCKET_SIZE; i++) {
		if (!_spanLists[i].Empty()) {
			Span* newSpan = new Span;
			Span* oldSpan = _spanLists[i].PopFront();
			newSpan->_pageID = oldSpan->_pageID;
			oldSpan->_pageID += pageSize;
			newSpan->_pageAmount = pageSize;
			oldSpan->_pageAmount -= pageSize;
			_spanLists[i - pageSize].PushFront(oldSpan);
			return newSpan;
		}
	}

	// 128个桶全为空，就要向系统申请内存了

	void* ptr = SystemAlloc(PAGE_CACHE_HASH_BUCKET_SIZE); // 使用系统调用接口申请内存PAGE_CACHE_HASH_BUCKET_SIZE页内存
	Span* biggestSpan = new Span; // 为最大块内存创建Span结构体
	biggestSpan->_freeList = ptr;
	biggestSpan->_pageID = ((PAGE_SIZE)ptr >> PAGE_SHIFT);
	biggestSpan->_pageAmount = PAGE_CACHE_HASH_BUCKET_SIZE;
	_spanLists[PAGE_CACHE_HASH_BUCKET_SIZE - 1].PushFront(biggestSpan); // 将Span放入带头双向循环链表

	return NewSpan(pageSize); // 递归调用NewSpan（复用）
}