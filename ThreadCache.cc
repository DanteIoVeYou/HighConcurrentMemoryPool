#include "ThreadCache.hpp"
#include "CentralCache.hpp"

void* ThreadCache::FetchFromCentralCache(size_t size, size_t index) {
	//.TODO
	// 向Central的下标为index的桶一批申请与size有关大小的内存：batchSize
	// 采用慢增长反馈调节法确定申请的一批内存块数，对于大内存，申请的少一点；对于小内存，申请的多一点

	size_t batchSize = SizeClass::ApplyBatchSize(size); // ApplyBatchSizeFromCentralCache计算batchSize
	batchSize = (std::min)(_freeLists[index]._MaxSize, batchSize); // batchSize和不断增长的_MaxSize比较，取较小值，保证一个批次大块内存的分配不会多，小块内存的分配一次比一次增多
	if (batchSize == _freeLists[index]._MaxSize) {
		_freeLists[index]._MaxSize++;
	}
	// 向index下标的桶里面取batchSize个size大小的内存块，return一个，(batchSize-1)挂到自由链表以备下次使用；start、end为输出型参数
	void* start = nullptr; // 申请到的内存的头n个字节地址
	void* end = nullptr; // 申请到的内存的尾n个字节地址
	size_t actualSize = CentralCache::GetSingleInstance()->FenchSpanPart(start, end, size, index, batchSize); // 向CentralCache的index桶申请batchSize个size大小的内存块
	assert(actualSize >= 1); // 获取了0个内存块断言错误；因为我是多申请的，所以能给几个给几个，但至少得给我一个
	if (actualSize == 1) { // 获取1个内存块，无需将多余申请的挂到自由链表
		return start;
	}
	else { // 拿多了的内存块挂到自由链表上，以备后续再次分配
		_freeLists[index].PushRange(NextObj(start), end, actualSize - 1); // start后一个~end
		return start;
	}
}

void* ThreadCache::Allocate(size_t size) { // 申请
	assert(size <= THREAD_CACHE_MAX_ALLOCATE_BYTES); // 如果申请的内存大于THREAD_CACHE_MAX_ALLOCATE_BYTES，就不该走这条路径
	size_t roundUpSize = SizeClass::RoundUp(size); // 根据索要的字节数求出对齐数
	size_t index = SizeClass::Index(size); // 根据索要的字节数求出 如此对齐数的内存块在哪个下标的哈希桶下面找
	if (_freeLists[index].Empty()) { // 所寻哈希桶下面没有现成的内存块
		// ...
		return FetchFromCentralCache(roundUpSize, index); // 去下一层————CentralCache申请
	}
	else {
		return _freeLists[index].Pop(); // 有现成的内存块，就弹出
	}
}

void ThreadCache::Deallocate(void* ptr, size_t size) { // 释放
	// TODO
	assert(ptr);
	assert(size <= THREAD_CACHE_MAX_ALLOCATE_BYTES); // 如果释放的内存大于THREAD_CACHE_MAX_ALLOCATE_BYTES，就不该走这条路径

	size_t index = SizeClass::Index(size); // 找到对应的桶
	_freeLists[index].Push(ptr); // 在编号为index的桶里面头插释放的内存块

	// 如果桶里的内存块太多，就释放掉一些到CentralCache中
	if (_freeLists[index].Size() >= _freeLists[index]._MaxSize) {
		FreeForListTooLong(_freeLists[index], index); 
	}
}

void ThreadCache::FreeForListTooLong(FreeList& list, size_t index) { // 向下一层CentralCache释放自由链表上的过多的内存块
	void* start = nullptr; // 输出型参数，用来表示释放的内存的范围
	void* end = nullptr;
	list.PopRange(start, end, list._MaxSize); // 弹出自由链表上的一片内存块
	// 将弹出的内存块还给Centrald的Span
	CentralCache::GetSingleInstance()->FreeFromThreadToSpan(start, end, index); // 把ThreadCache释放的资源挂到CentralCache的Span里面去，index一一对应
}