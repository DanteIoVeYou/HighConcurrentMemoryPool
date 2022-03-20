#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include <ctime>
#include <vector>
using std::cout;
using std::endl;
template<class T>
class ObjectPool
{
public:
	ObjectPool()
		:_memory(nullptr)
		,_remainSize(0)
		,_freeList(nullptr)
	{}
	~ObjectPool() {}
	T* New() {
		if (_freeList != nullptr) { // 当_freeList不为空时,优先向_freeList申请空间
			T* obj = (T*)_freeList;
			_freeList = (T*)(*(void**)_freeList);
			_remainSize -= sizeof(T);
			new(obj) T;
			return obj;
		}
		else { // 当_freeList为空时，只能向内存池申请空间
			if (_remainSize < sizeof(T)) { // 当前内存池首次开辟或者是剩余内存池中的内存不够开一个T大小的空间时，内存池就向系统申请一块新的空间
				_memory = (char*)malloc(128 * 1024);
				_remainSize = 128 * 1024;
			}
			// 从内存池中可以拿出T类型大小的内存
			T* obj = (T*)_memory;
			_memory += sizeof(T);
			_remainSize -= sizeof(T);
			new(obj) T;
			return obj;
		}

	}
	void Delete(T* obj) {
		if (_freeList == nullptr) { // 当_freeList 内没有存放释放的内存块的时候
			_freeList = obj;
			*(void**)obj = nullptr;
		}
		else { // 当_freeList 内已经有了被释放的内存块的时候，头插新的内存块
			*(void**)obj = _freeList;
			_freeList = obj;
		}
	}
private:
	char* _memory; // 向系统申请的内存池
	size_t _remainSize; // 内存池的剩余字节数
	void* _freeList; // 使用链表管理的释放的内存：前4/8个字节存储下一个块的地址
};


//void test1() {
//	const int N = 10000;
//	int begin1 = clock();
//	for (int i = 0; i < N; i++) {
//		ObjectPool<int> op;
//	}
//	int end1 = clock();
//	int time1 = end1 - begin1;
//	
//	int begin2 = clock();
//	//for (int i = 0; i < N; i++) {
//	//	for (int j = 0; j < 128 * 1024 / 4; j++) {
//	//		new int;
//	//		delete 
//	//	}
//	//}
//	int end2 = clock();
//	int time2 = end2 - begin2;
//	std::cout << "time1: " << time1 << " " << "time2: " << time2 << std::endl;
//}



struct TreeNode
{
	int _val;
	TreeNode* _left;
	TreeNode* _right;
	TreeNode()
		:_val(0)
		, _left(nullptr)
		, _right(nullptr)
	{}
};
void TestObjectPool()
{
	// 申请释放的轮次
	const size_t Rounds = 3;
	// 每轮申请释放多少次
	const size_t N = 100000;
	size_t begin1 = clock();
	std::vector<TreeNode*> v1;
	v1.reserve(N);
	for (size_t j = 0; j < Rounds; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			v1.push_back(new TreeNode);
		}
		for (int i = 0; i < N; ++i)
		{
			delete v1[i];
		}
		v1.clear();
	}
	size_t end1 = clock();
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	ObjectPool<TreeNode> TNPool;
	size_t begin2 = clock();
	std::vector<TreeNode*> v2;
	v2.reserve(N);
	for (size_t j = 0; j < Rounds; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			v2.push_back(TNPool.New());
		}
		for (int i = 0; i < 100000; ++i)
		{
			TNPool.Delete(v2[i]);
		}
		v2.clear();
	}
	size_t end2 = clock();
	cout << "new cost time:" << end1 - begin1 << endl;
	cout << "object pool cost time:" << end2 - begin2 << endl;
}
