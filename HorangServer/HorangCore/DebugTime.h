#pragma once
#include "Types.h"
#include <chrono>

namespace Horang
{
	class DebugTime
	{
	public:
		static void TickStart();
		static void TickEnd();
		static void TickPrint();
		static void TickEndPrint();

	private:
		static uint64 _startTick;
		static uint64 _endTick;

	public:
		static void TimeStart();
		static void TimeEnd();
		static void TimePrint();
		static void TimeEndPrint();

	private:
		static std::chrono::high_resolution_clock::time_point _startTime;
		static std::chrono::high_resolution_clock::time_point _endTime;

	};

	extern DebugTime GDebugTime;
}