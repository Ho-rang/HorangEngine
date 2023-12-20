#pragma once

class TestLockClass
{
public:
	int32 TestRead();
	void TestPush();
	void TestPop();

	int32 TestMutexRead();
	void TestMutexPush();
	void TestMutexPop();

private:
	HORANG_LOCK;
	std::mutex _mutex;

	std::queue<int32> _queue;
};