#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "GlobalQueue.h"
#include "JobTimer.h"

using namespace Horang;

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(std::function<void(void)> callback)
{
	LockGuard guard(_lock);

	_threads.push_back(std::thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	for (std::thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{

}

void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		if (::GetTickCount64() > LEndTickCount)
			break;

		JobQueueRef jobQueue = GGlobalQueue->Pop();
		if (jobQueue == nullptr)
			break;

		jobQueue->Execute();
	}
}

void ThreadManager::DistributeReservedJobs()
{
	const uint64 now = ::GetTickCount64();

	GJobTimer->Distribute(now);
}
