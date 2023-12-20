#pragma once

#define _CRASH

#ifdef _DEBUG

#define CRASH(casue)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#ifdef _CRASH

#define ASSERT_CRASH(expression)			\
{											\
	if(!(expression))						\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expression);		\
	}										\
}
#else
#include <cassert>
#define ASSERT_CRASH(expression)			\
{											\
	assert(expression);						\
	__analysis_assume(expression);			\
}
#endif

#else

#define CRASH(casue)

#define ASSERT_CRASH(expression)

#endif // _DEBUG

#define USE_MANY_LOCKS(count) Horang::Lock _locks[count];
#define USE_LOCK USE_MANY_LOCKS(1);

#define READ_LOCK_IDX(idx) \
Horang::ReadLockGuard readLockGuard_##idx(_locks[idx]);
#define READ_LOCK	READ_LOCK_IDX(0)

#define WRITE_LOCK_IDX(idx) \
Horang::WriteLockGuard writeLockGuard_##idx(_locks[idx]);
#define WRITE_LOCK	WRITE_LOCK_IDX(0)

#define HORANG_LOCK Horang::Lock _lock;
#define HORANG_READ_LOCK Horang::ReadLockGuard readLockGuard(_lock);
#define HORANG_WRITE_LOCK Horang::WriteLockGuard writeLockGuard(_lock);