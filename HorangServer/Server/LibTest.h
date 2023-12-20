#pragma once
#include "Lock.h"

class LibTest
{
public:
	LibTest();
	~LibTest();

	void Check();

private:

	void TestThreadManager();
	void TestLock();

private:
	
};

