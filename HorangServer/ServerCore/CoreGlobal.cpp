#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "DBConnectionPool.h"

using namespace Horang;

Horang::ThreadManager* GThreadManager = nullptr;
Horang::Memory* GMemory = nullptr;
Horang::SendBufferManager* GSendBufferManager = nullptr;


Horang::DeadLockProfiler* GDeadLockProfiler = nullptr;

Horang::DBConnectionPool* GDBConnectionPool = nullptr;

class CoreGlobal
{
public:
	CoreGlobal();
	~CoreGlobal();

private:
} GCoreGlobal;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new Horang::ThreadManager();
	GMemory = new Horang::Memory();
	GSendBufferManager = new Horang::SendBufferManager();


	GDeadLockProfiler = new Horang::DeadLockProfiler();

	GDBConnectionPool = new Horang::DBConnectionPool();

	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
	delete GMemory;
	delete GSendBufferManager;


	delete GDeadLockProfiler;

	delete GDBConnectionPool;

	SocketUtils::Clear();
}
