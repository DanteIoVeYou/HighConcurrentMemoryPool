#include "ThreadCache.hpp"
#include "CentralCache.hpp"
void* ThreadCache::FenchFromCentralCahce(size_t size, size_t index) {
	//.TODO
	// 向Central的下标为index的桶一批申请与size有关大小的内存：batchSize
	// 采用慢增长反馈调节法确定申请的一批内存块数，对于大内存，申请的少一点；对于小内存，申请的多一点

	size_t batchSize = SizeClass::ApplyBatchSizeFromCentralCache(size); // ApplyBatchSizeFromCentralCache计算batchSize
	batchSize = std::min(_MaxSize, batchSize); // batchSize和不断增长的_MaxSize比较，取较小值，保证一个批次大块内存的分配不会多，小块内存的分配一次比一次增多
	// 向index下标的桶里面取batchSize个size大小的内存块
	void* start = nullptr; // 申请到的内存的头n个字节地址
	void* end = nullptr; // 申请到的内存的尾n个字节地址
	CentralCache::FenchSpanPart(start, end, size, index, batchSize);
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