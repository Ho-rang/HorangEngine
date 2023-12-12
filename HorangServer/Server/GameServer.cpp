#include "pch.h"
#include "DebugTime.h"

using Ho::DebugTime;

int main()
{
	DebugTime::TimeStart();
	DebugTime::TimeEnd();
	DebugTime::TimePrint();

	return 0;
}