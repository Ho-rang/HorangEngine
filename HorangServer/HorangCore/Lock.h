#pragma once
#include "Types.h"

namespace Horang
{
	class Lock
	{
		enum : uint32
		{
			ACQUIRE_TIMEOUT_TICK = 10000,
			MAX_SPIN_COUNT = 5000,
			WRITE_THREAD_MASK = 0xFFFF'0000,
			READ_COUNT_MASK = 0x0000'FFFF,
			EMPTY_FLAG = 0x0000'0000,
		};

	public:
		void WriteLock();
		void WriteUnLock();
		void ReadLock();
		void ReadUnLock();

	private:
		Atomic<uint32> _lockFlag = EMPTY_FLAG;

		// 재귀적으로 동일한 Lock을 잡았을 때 허용해주기 위함
		uint32 _writeCount = 0;
	};

	// RAII 패턴
	// 스택에 할당된 메모리는 자동으로 해제되는 것을 활용

	class ReadLockGuard
	{
	public:
		ReadLockGuard(Lock& lock)
			: _lock(lock)
		{
			_lock.ReadLock();
		}
		~ReadLockGuard()
		{
			_lock.ReadUnLock();
		}

	private:
		Lock& _lock;
	};

	class WriteLockGuard
	{
	public:
		WriteLockGuard(Lock& lock)
			: _lock(lock)
		{
			_lock.WriteLock();
		}
		~WriteLockGuard()
		{
			_lock.WriteUnLock();
		}

	private:
		Lock& _lock;
	};
}