#include "pch.h"
#include "ThreadManager.h"

using namespace Horang;

ThreadManager::ThreadManager()
{
	// MainThread 에서 동작
	this->InitTLS();
}

ThreadManager::~ThreadManager()
{
	ThreadManager::Join();
	this->Join();
}

void ThreadManager::Launch(std::function<void(void)> callback)
{
	LockGuard guard(_lock);

	_threads.push_back(std::thread([=]()
		{
			// thread 초기화 후 실행 완료되면 정리
			this->InitTLS();
			callback();
			this->DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	LockGuard guard(_lock);

	for (auto& thread : _threads)
	{
		if(thread.joinable())
			thread.join();
	}

	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<int> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{

}
