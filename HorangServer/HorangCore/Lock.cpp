#include "pch.h"
#include "Lock.h"

using namespace Horang;

// Write를 잡고 있을 때 Read lock 허용
// Read Lock을 잡고 있을 때 Write lock 불가 - Read lock은 여러 thread가 잡고 있을 수 있으므로

void Lock::WriteLock()
{
	// 현재 Lock을 잡고있는 Thread가 나라면 성공
	const uint32 currentLockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (currentLockThreadId == LThreadId)
	{
		_writeCount++;
		return;
	}

	// Write Thread ID Setting
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);

	const int64 beginTick = ::GetTickCount64();
	// spinLock으로 소유권 얻기
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_weak(expected, desired))
			{
				_writeCount++;
				return;
			}
		}

		// Lock을 얻기 위해 경합한 시간이 넘었다면 에러
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("Write Lock Time Out");

		std::this_thread::yield();
	}
}

void Lock::WriteUnLock()
{
	// ReadLock을 풀기 전에는 Write unlock 불가능
	if ((_lockFlag.load() & READ_COUNT_MASK) != EMPTY_FLAG)
		CRASH("Invalid Unlock Order");

	const int32 lockCount = --_writeCount;

	if (lockCount == 0)
	{
		_lockFlag.store(EMPTY_FLAG);
	}
}

void Lock::ReadLock()
{
	// 현재 Lock을 잡고있는 Thread가 나라면 성공
	const uint32 currentLockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (currentLockThreadId == LThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// 아무도 소유하고 있지 않을 때 경합해서 공유 카운트 증가
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(expected, expected + 1))
				return;
		}

		// Lock을 얻기 위해 경합한 시간이 넘었다면 에러
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("Read Lock Time Out");

		std::this_thread::yield();
	}
}

void Lock::ReadUnLock()
{
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("Multiple Unlock");
}
