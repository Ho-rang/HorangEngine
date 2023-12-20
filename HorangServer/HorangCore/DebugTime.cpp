#include "pch.h"
#include "DebugTime.h"
#include <string>

using namespace Horang;

DebugTime GDebugTime;

uint64 DebugTime::_startTick = 0;
uint64 DebugTime::_endTick = 0;

std::chrono::high_resolution_clock::time_point DebugTime::_startTime;
std::chrono::high_resolution_clock::time_point DebugTime::_endTime;


void DebugTime::TimeStart()
{
	_startTime = std::chrono::high_resolution_clock::now();
	_endTime = _startTime;
}

void DebugTime::TimeEnd()
{
	_endTime = std::chrono::high_resolution_clock::now();
}

void DebugTime::TimePrint(std::string str)
{
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(_endTime - _startTime).count();

	if (str.length() > 0)
		std::cout << str << std::endl;

	std::cout << "Execution Time: " << duration << " microseconds." << std::endl;
}

void DebugTime::TimeEndPrint(std::string str)
{
	DebugTime::TimeEnd();
	DebugTime::TimePrint(str);
}

void DebugTime::TickStart()
{
	_startTick = ::GetTickCount64();
	_endTick = _startTick;
}

void DebugTime::TickEnd()
{
	_endTick = ::GetTickCount64();
}

void DebugTime::TickPrint()
{
	std::cout << "Elapsed Tick : " << (_endTick - _startTick) << std::endl;
}

void DebugTime::TickEndPrint()
{
	DebugTime::TickEnd();
	DebugTime::TickPrint();
}