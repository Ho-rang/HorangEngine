#include "pch.h"
#include "TestLockClass.h"

int32 TestLockClass::TestRead()
{
	HORANG_READ_LOCK;

	if (_queue.empty())
		return -1;

	return _queue.front();
}

void TestLockClass::TestPush()
{
	HORANG_WRITE_LOCK;

	_queue.push(rand() % 100);
}

void TestLockClass::TestPop()
{
	HORANG_WRITE_LOCK;

	if (!_queue.empty())
		_queue.pop();
}

int32 TestLockClass::TestMutexRead()
{
	std::lock_guard lock(_mutex);

	if (_queue.empty())
		return -1;

	return _queue.front();
}

void TestLockClass::TestMutexPush()
{
	std::lock_guard lock(_mutex);

	_queue.push(rand() % 100);
}

void TestLockClass::TestMutexPop()
{
	std::lock_guard lock(_mutex);

	if (!_queue.empty())
		_queue.pop();
}
