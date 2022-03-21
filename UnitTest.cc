#define _CRT_SECURE_NO_WARNINGS 1
#include "ObjectPool.hpp"
#include "ConcurrentAlloc.hpp"
/// //////////////////////////////////////////////////////////////////////////////////ThreadLibTest
void Alloc1() {
	for (size_t i = 0; i < 10;  i++) {
		//void* ptr = ConcurrentAlloc(8);

		std::cout << "thread1: " << i << "threadID: " << std::this_thread::get_id() << std::endl;
	}
}

void Alloc2() {
	for (size_t i = 0; i < 10; i++) {
		//void* ptr = ConcurrentAlloc(7);

		std::cout << "thread2: " << i << "threadID: " << std::this_thread::get_id() << std::endl;

	}
}
void ConcurrentAllocTest1() {
	std::thread t1(Alloc1);
	t1.join();

	std::thread t2(Alloc2);

	t2.join();
}
/// //////////////////////////////////////////////////////////////////////////////////ThreadLibTest

int main()
{
	ConcurrentAllocTest1();
	return 0;
}