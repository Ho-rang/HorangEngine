#include "pch.h"
#include "Lock.h"

using namespace Horang;

// Write�� ��� ���� �� Read lock ���
// Read Lock�� ��� ���� �� Write lock �Ұ� - Read lock�� ���� thread�� ��� ���� �� �����Ƿ�

void Lock::WriteLock()
{
	// ���� Lock�� ����ִ� Thread�� ����� ����
	const uint32 currentLockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (currentLockThreadId == LThreadId)
	{
		_writeCount++;
		return;
	}

	// Write Thread ID Setting
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);

	const int64 beginTick = ::GetTickCount64();
	// spinLock���� ������ ���
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

		// Lock�� ��� ���� ������ �ð��� �Ѿ��ٸ� ����
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("Write Lock Time Out");

		std::this_thread::yield();
	}
}

void Lock::WriteUnLock()
{
	// ReadLock�� Ǯ�� ������ Write unlock �Ұ���
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
	// ���� Lock�� ����ִ� Thread�� ����� ����
	const uint32 currentLockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (currentLockThreadId == LThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// �ƹ��� �����ϰ� ���� ���� �� �����ؼ� ���� ī��Ʈ ����
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(expected, expected + 1))
				return;
		}

		// Lock�� ��� ���� ������ �ð��� �Ѿ��ٸ� ����
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
