#include "CentralCache.hpp"
CentralCache* CentralCache::GetSingleInstance() {
	return &_singleInstance;
}


void CentralCache::FenchSpanPart(void*& start, void*& end, size_t size, size_t index, size_t batchSize) {
	void* span = GetOneSpan(size, index, batchSize);
	if()
	start = 
}
