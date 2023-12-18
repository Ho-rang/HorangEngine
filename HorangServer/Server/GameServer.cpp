#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

#include <windows.h>
#include <future>

Horang::CoreGlobal core;


void ThreadMain()
{
	while (true)
	{
		cout << LThreadId << endl;
		std::this_thread::sleep_for(1s);
	}
}

int main()
{
	for (int32 i = 0; i < 5; i++)
	{
		Horang::GThreadManager->Launch([]() {ThreadMain(); });
	}

	Horang::GThreadManager->Join();

	return 0;
}