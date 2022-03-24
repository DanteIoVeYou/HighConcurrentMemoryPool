#include "CentralCache.hpp"
#include "PageCache.hpp"
CentralCache CentralCache::_singleInstance;
Span* CentralCache::GetOneSpan(SpanList& list, size_t size) { // ȥ��Ӧ�Ĺ�ϣͰ��������û��Span
	//TODO

	size_t batchSize = SizeClass::ApplyBatchSize(size);
	size_t sizeAllAmount = batchSize * size;
	size_t index = SizeClass::Index(size);
	// ��CentralCache��SpanList������Span
	// ���SpanList���ֳɵ�Span��ֱ�ӷ���
	Span* cur = list.Begin();
	while (cur != list.End()) {
		if (cur->_freeList == nullptr) {
			cur = cur->_next;
		}
		else {
			return cur;
		}
	}

	// ���SpanListû���ֳɵ�Span����PageCache������Ե�Spanҳ
	
	size_t pageSize = SizeClass::ApplyPageSize(sizeAllAmount);
	// ������ʱ��������������߳��ͷ��ڴ棬�������
	list._mtx.unlock();
	PageCache::GetInstance()->_mtx.lock();
	Span* span = PageCache::GetInstance()->NewSpan(pageSize); // ��PageCache������һ���������ɸ�ҳ��Span
	PageCache::GetInstance()->_mtx.unlock();


	// ��Span�г����ɸ�size��С��С�ڴ��β�嵽����������
	char* begin = (char*)(span->_pageID << PAGE_SHIFT); // ͨ��sapn����ҳҳ���ҵ��ڴ��ַ
	char* end = begin + (span->_pageAmount << PAGE_SHIFT); // �ҵ��ڴ�β��
	span->_freeList = (void*)begin;
	void* tail = span->_freeList; // ��������β��
	begin += size;
	while (begin < end) {
		 // ��ҳ�и�Ϊsize��Ԫ��С��С�ڴ��
		NextObj(tail) = begin;
		tail = NextObj(tail);
		begin += size;
	}

	// ��ʣ�µ��ڴ����list
	list._mtx.lock();
	list.Insert(list.Begin(), span);
	return span;
	
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


void CentralCache::FreeFromThreadToSpan(void* start, void* end, size_t index) {
	spanLists[index]._mtx.lock(); // Ͱ��

	while (start) {
		void* next = NextObj(start);
		Span* span = PageCache::GetInstance()->GetSpanViaAddress(start);
		NextObj(start) = span->_freeList;
		span->_freeList = start;
		span->_usedCount--;
		if (span->_usedCount == 0) { // Span��ҳȫ���黹
			spanLists[index].Erase(span);
			span->_prev = nullptr;
			span->_next = nullptr;
			span->_freeList = nullptr;

			spanLists[index]._mtx.unlock();
			PageCache::GetInstance()->_mtx.lock();
			PageCache::GetInstance()->GiveSpanToPage(span);
			PageCache::GetInstance()->_mtx.unlock();
			spanLists[index]._mtx.lock();
		}
		start = next;
	}


	spanLists[index]._mtx.unlock(); // �⿪Ͱ��
}