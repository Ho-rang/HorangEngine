#pragma once

#define WIN32_LEAN_AND_MEAN	// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib,"Debug\\HorangCore.lib")
#else
#pragma comment(lib,"Release\\HorangCore.lib")
#endif

#include "CorePch.h"
