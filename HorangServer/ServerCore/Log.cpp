#include "pch.h"
#include "Log.h"

HANDLE Horang::Log::_stdOut = INVALID_HANDLE_VALUE;

Horang::Log::Log()
{
	Log::_stdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
}

Horang::Log::~Log()
{
	::CloseHandle(Log::_stdOut);
}

void Horang::Log::SetColor(LogColor color)
{
	static Horang::LogColor nowColor = Horang::LogColor::WHITE;

	if (nowColor == color)
		return;

	nowColor = color;

	static WORD SColors[]
	{
		0,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_RED | FOREGROUND_INTENSITY,
		FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
	};

	::SetConsoleTextAttribute(_stdOut, SColors[static_cast<int32>(nowColor)]);
}

Horang::LogStream Horang::DebugLog(LogColor color /*= Color::WHITE*/)
{
	return LogStream(color);
}
