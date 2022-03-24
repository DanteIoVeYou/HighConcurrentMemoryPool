#include "CentralCache.hpp"
#include "PageCache.hpp"
CentralCache CentralCache::_singleInstance;
Span* CentralCache::GetOneSpan(SpanList& list, size_t size) { // 去对应的哈希桶下面找有没有Span
	//TODO

	size_t batchSize = SizeClass::ApplyBatchSize(size);
	size_t sizeAllAmount = batchSize * size;
	size_t index = SizeClass::Index(size);
	// 从CentralCache的SpanList里面拿Span
	// 如果SpanList有现成的Span，直接返回
	Span* cur = list.Begin();
	while (cur != list.End()) {
		if (cur->_freeList == nullptr) {
			cur = cur->_next;
		}
		else {
			return cur;
		}
	}

	// 如果SpanList没有现成的Span，向PageCache申请相对的Span页
	
	size_t pageSize = SizeClass::ApplyPageSize(sizeAllAmount);
	// 在这暂时解锁，如果其他线程释放内存，不会堵塞
	list._mtx.unlock();
	PageCache::GetInstance()->_mtx.lock();
	Span* span = PageCache::GetInstance()->NewSpan(pageSize); // 从PageCache中申请一个含有若干个页的Span
	PageCache::GetInstance()->_mtx.unlock();


	// 将Span切成若干个size大小的小内存块尾插到自由链表中
	char* begin = (char*)(span->_pageID << PAGE_SHIFT); // 通过sapn的首页页号找到内存地址
	char* end = begin + (span->_pageAmount << PAGE_SHIFT); // 找到内存尾部
	span->_freeList = (void*)begin;
	void* tail = span->_freeList; // 自由链表尾部
	begin += size;
	while (begin < end) {
		 // 将页切割为size单元大小的小内存块
		NextObj(tail) = begin;
		tail = NextObj(tail);
		begin += size;
	}

	// 把剩下的内存放入list
	list._mtx.lock();
	list.Insert(list.Begin(), span);
	return span;
	
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


void CentralCache::FreeFromThreadToSpan(void* start, void* end, size_t index) {
	spanLists[index]._mtx.lock(); // 桶锁

	while (start) {
		void* next = NextObj(start);
		Span* span = PageCache::GetInstance()->GetSpanViaAddress(start);
		NextObj(start) = span->_freeList;
		span->_freeList = start;
		span->_usedCount--;
		if (span->_usedCount == 0) { // Span的页全部归还
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


	spanLists[index]._mtx.unlock(); // 解开桶锁
}