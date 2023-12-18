#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

using namespace Horang;

ThreadManager* Horang::GThreadManager = nullptr;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
}