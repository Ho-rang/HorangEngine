#pragma once

#include <thread>
#include <functional>

namespace Horang
{
	/*
		ThreadManager
	*/
	class ThreadManager
	{
	public:
		ThreadManager();
		~ThreadManager();

		void Launch(std::function<void(void)> callback);
		void Join();

		static void InitTLS();
		static void DestroyTLS();

		static void DoGlobalQueueWork();
		static void DistributeReservedJobs();
	private:
		Mutex _lock;
		std::vector<std::thread> _threads;
	};
}