#include "ThreadCache.hpp"
#include "CentralCache.hpp"

void* ThreadCache::FenchFromCentralCahce(size_t size, size_t index) {
	//.TODO
	// 向Central的下标为index的桶一批申请与size有关大小的内存：batchSize
	// 采用慢增长反馈调节法确定申请的一批内存块数，对于大内存，申请的少一点；对于小内存，申请的多一点

	size_t batchSize = SizeClass::ApplyBatchSizeFromCentralCache(size); // ApplyBatchSizeFromCentralCache计算batchSize
	batchSize = std::min(_freeLists[index]._MaxSize, batchSize); // batchSize和不断增长的_MaxSize比较，取较小值，保证一个批次大块内存的分配不会多，小块内存的分配一次比一次增多
	if (batchSize == _freeLists[index]._MaxSize++) {
		_freeLists[index]._MaxSize++;
	}
	// 向index下标的桶里面取batchSize个size大小的内存块
	void* start = nullptr; // 申请到的内存的头n个字节地址
	void* end = nullptr; // 申请到的内存的尾n个字节地址
	size_t actualSize = CentralCache::GetSingleInstance()->FenchSpanPart(start, end, size, index, batchSize);
	assert(actualSize >= 1); // 获取了0个内存块断言错误
	if (actualSize == 1) { // 获取1个内存块，无需将多余申请的挂到自由链表
		return start;
	}
	else { // 拿多了的内存块挂到自由链表上，以备后续再次分配
		_freeLists[index].PushRange(NextObj(start), end);
		return start;
	}
}

void* ThreadCache::Allocate(size_t size) {
	assert(size <= THREAD_CACHE_MAX_ALLOCATE_BYTES); // 如果申请的内存大于THREAD_CACHE_MAX_ALLOCATE_BYTES，就不该走这条路径
	size_t roundUpSize = SizeClass::RoundUp(size);
	size_t index = SizeClass::Index(size);
	if (_freeLists[index].Empty()) {
		// ...
		return FenchFromCentralCahce(size, index);
	}
	else {
		return _freeLists[index].Pop();
	}
}

void ThreadCache::Deallocate(void* ptr, size_t size) {
	// TODO
	assert(ptr);
	assert(size <= THREAD_CACHE_MAX_ALLOCATE_BYTES); // 如果释放的内存大于THREAD_CACHE_MAX_ALLOCATE_BYTES，就不该走这条路径

	size_t index = SizeClass::Index(size); // 找到对应的桶
	_freeLists[index].Push(ptr); // 在编号为index的桶里面头插释放的内存块
}