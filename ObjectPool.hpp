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
		if (_freeList != nullptr) { // ��_freeList��Ϊ��ʱ,������_freeList����ռ�
			T* obj = (T*)_freeList;
			_freeList = (T*)(*(void**)_freeList);
			_remainSize -= sizeof(T);
			new(obj) T;
			return obj;
		}
		else { // ��_freeListΪ��ʱ��ֻ�����ڴ������ռ�
			if (_remainSize < sizeof(T)) { // ��ǰ�ڴ���״ο��ٻ�����ʣ���ڴ���е��ڴ治����һ��T��С�Ŀռ�ʱ���ڴ�ؾ���ϵͳ����һ���µĿռ�
				_memory = (char*)malloc(128 * 1024);
				_remainSize = 128 * 1024;
			}
			// ���ڴ���п����ó�T���ʹ�С���ڴ�
			T* obj = (T*)_memory;
			_memory += sizeof(T);
			_remainSize -= sizeof(T);
			new(obj) T;
			return obj;
		}

	}
	void Delete(T* obj) {
		if (_freeList == nullptr) { // ��_freeList ��û�д���ͷŵ��ڴ���ʱ��
			_freeList = obj;
			*(void**)obj = nullptr;
		}
		else { // ��_freeList ���Ѿ����˱��ͷŵ��ڴ���ʱ��ͷ���µ��ڴ��
			*(void**)obj = _freeList;
			_freeList = obj;
		}
	}
private:
	char* _memory; // ��ϵͳ������ڴ��
	size_t _remainSize; // �ڴ�ص�ʣ���ֽ���
	void* _freeList; // ʹ�����������ͷŵ��ڴ棺ǰ4/8���ֽڴ洢��һ����ĵ�ַ
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
	// �����ͷŵ��ִ�
	const size_t Rounds = 3;
	// ÿ�������ͷŶ��ٴ�
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
