#include "pch.h"
#include "LibTest.h"

#include "DebugTime.h"

#include "ThreadManager.h"

#include "TestLockClass.h"

LibTest::LibTest()
{

}

LibTest::~LibTest()
{

}

void LibTest::Check()
{
	//this->TestThreadManager();
	TestLock();
}

void LibTest::TestThreadManager()
{
	for (int32 i = 0; i < 5; i++)
	{
		Horang::GThreadManager->Launch([]()
			{
				for (int32 i = 0; i < 10; i++)
				{
					cout << "Thread ID : " << LThreadId << " " << endl;
					std::this_thread::sleep_for(1s);
				}
			}
		);
	}

	Horang::GThreadManager->Join();
}

void LibTest::TestLock()
{
	// User Lock
	{
		TestLockClass testLock;

		Horang::DebugTime::TimeStart();
		Horang::DebugTime::TickStart();

		for (int32 i = 0; i < 10; i++)
		{
			Horang::GThreadManager->Launch([&]()
				{
					for (int32 i = 0; i < 1000; i++)
					{
						testLock.TestPush();
						//std::this_thread::sleep_for(1ms);
						testLock.TestPop();
					}
				}
			);
		}

		for (int32 i = 0; i < 10; i++)
		{
			Horang::GThreadManager->Launch([&]()
				{
					for (int32 i = 0; i < 1000000; i++)
					{
						int32 value = testLock.TestRead();
						//std::this_thread::sleep_for(1ms);
					}
				}
			);
		}

		Horang::GThreadManager->Join();
		Horang::DebugTime::TimeEndPrint("User Lock Time");
		Horang::DebugTime::TickEndPrint();
	}

	// Kernel Lock
	{
		TestLockClass testLock;

		Horang::DebugTime::TimeStart();
		Horang::DebugTime::TickStart();

		for (int32 i = 0; i < 10; i++)
		{
			Horang::GThreadManager->Launch([&]()
				{
					for (int32 i = 0; i < 1000; i++)
					{
						testLock.TestMutexPush();
						//std::this_thread::sleep_for(1ms);
						testLock.TestMutexPop();
					}
				}
			);
		}

		for (int32 i = 0; i < 10; i++)
		{
			Horang::GThreadManager->Launch([&]()
				{
					for (int32 i = 0; i < 1000000; i++)
					{
						int32 value = testLock.TestMutexRead();
						//std::this_thread::sleep_for(1ms);
					}
				}
			);
		}

		Horang::GThreadManager->Join();
		Horang::DebugTime::TimeEndPrint("Kernel Lock Time");
		Horang::DebugTime::TickEndPrint();
	}
}
