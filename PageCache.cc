#include "PageCache.hpp"
#include "CentralCache.hpp"
PageCache PageCache::_singleInstance;
Span* PageCache::NewSpan(size_t pageSize) {
	assert(pageSize >= 1 && pageSize <= PAGE_CACHE_HASH_BUCKET_SIZE);


	if (!_spanLists[pageSize - 1].Empty()) {
		return _spanLists[pageSize-1].PopFront();
	}

	// pageSize-1�±��Ͱ��Span�������pageSize��ʼ������Ͱ�д��
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

	// 128��ͰȫΪ�գ���Ҫ��ϵͳ�����ڴ���

	void* ptr = SystemAlloc(PAGE_CACHE_HASH_BUCKET_SIZE); // ʹ��ϵͳ���ýӿ������ڴ�PAGE_CACHE_HASH_BUCKET_SIZEҳ�ڴ�
	Span* biggestSpan = new Span; // Ϊ�����ڴ洴��Span�ṹ��
	biggestSpan->_freeList = ptr;
	biggestSpan->_pageID = ((PAGE_SIZE)ptr >> PAGE_SHIFT);
	biggestSpan->_pageAmount = PAGE_CACHE_HASH_BUCKET_SIZE;
	_spanLists[PAGE_CACHE_HASH_BUCKET_SIZE - 1].PushFront(biggestSpan); // ��Span�����ͷ˫��ѭ������

	return NewSpan(pageSize); // �ݹ����NewSpan�����ã�
}