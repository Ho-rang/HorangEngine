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
